#include <algine/std/model/ShapeBuilder.h>
#include <algine/std/animation/BoneInfo.h>

#include <algine/core/texture/Texture2D.h>
#include <algine/core/PtrMaker.h>
#include <algine/core/TypeRegistry.h>
#include <algine/core/log/Log.h>

#include <algine/core/io/IOSystem.h>
#include <algine/core/io/IOStream.h>

#include <assimp/IOSystem.hpp>
#include <assimp/IOStream.hpp>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <tulz/Path.h>

#include <cfloat>

#include "../assimp2glm.h"

using namespace tulz;
using namespace std;

namespace algine {
namespace Default {
constexpr uint BonesPerVertex = 4;
constexpr auto ClassName = "Shape";
}

constexpr auto LOG_TAG = "ShapeBuilder";

ShapeBuilder::ShapeBuilder()
    : m_className(Default::ClassName),
      m_bonesPerVertex(Default::BonesPerVertex),
      m_shape(nullptr) {}

void ShapeBuilder::addParam(Param param) {
    m_params.emplace_back(param);
}

void ShapeBuilder::addParams(const vector<Param> &params) {
    for (auto param : params) {
        m_params.emplace_back(param);
    }
}

void ShapeBuilder::setParams(const vector<Param> &params) {
    m_params = params;
}

void ShapeBuilder::addInputLayoutLocations(const InputLayoutShapeLocations &locations) {
    m_locations.emplace_back(locations);
}

void ShapeBuilder::setInputLayoutLocations(const vector<InputLayoutShapeLocations> &locations) {
    m_locations = locations;
}

void ShapeBuilder::setModelPath(const string &path) {
    m_modelPath = path;
}

void ShapeBuilder::setAMTL(const AMTLManager &amtlManager) {
    m_amtlManager = amtlManager;
}

void ShapeBuilder::setBonesPerVertex(uint bonesPerVertex) {
    m_bonesPerVertex = bonesPerVertex;
}

void ShapeBuilder::setClassName(const string &name) {
    m_className = name;
}

const vector<ShapeBuilder::Param>& ShapeBuilder::getParams() const {
    return m_params;
}

const vector<InputLayoutShapeLocations>& ShapeBuilder::getInputLayoutLocations() const {
    return m_locations;
}

const string& ShapeBuilder::getModelPath() const {
    return m_modelPath;
}

const AMTLManager& ShapeBuilder::getAMTL() const {
    return m_amtlManager;
}

uint ShapeBuilder::getBonesPerVertex() const {
    return m_bonesPerVertex;
}

const string& ShapeBuilder::getClassName() const {
    return m_className;
}

const ShapeBuilder::BufferData<float>& ShapeBuilder::getVertices() const {
    return m_vertices;
}

void ShapeBuilder::setVertices(const BufferData<float> &vertices) {
    m_vertices = vertices;
}

const ShapeBuilder::BufferData<float>& ShapeBuilder::getNormals() const {
    return m_normals;
}

void ShapeBuilder::setNormals(const BufferData<float> &normals) {
    m_normals = normals;
}

const ShapeBuilder::BufferData<float>& ShapeBuilder::getTexCoords() const {
    return m_texCoords;
}

void ShapeBuilder::setTexCoords(const BufferData<float> &texCoords) {
    m_texCoords = texCoords;
}

const ShapeBuilder::BufferData<float>& ShapeBuilder::getTangents() const {
    return m_tangents;
}

void ShapeBuilder::setTangents(const BufferData<float> &tangents) {
    m_tangents = tangents;
}

const ShapeBuilder::BufferData<float>& ShapeBuilder::getBitangents() const {
    return m_bitangents;
}

void ShapeBuilder::setBitangents(const BufferData<float> &bitangents) {
    m_bitangents = bitangents;
}

const ShapeBuilder::BufferData<float>& ShapeBuilder::getBoneWeights() const {
    return m_boneWeights;
}

void ShapeBuilder::setBoneWeights(const BufferData<float> &boneWeights) {
    m_boneWeights = boneWeights;
}

const ShapeBuilder::BufferData<uint>& ShapeBuilder::getBoneIds() const {
    return m_boneIds;
}

void ShapeBuilder::setBoneIds(const BufferData<uint> &boneIds) {
    m_boneIds = boneIds;
}

const ShapeBuilder::BufferData<uint>& ShapeBuilder::getIndices() const {
    return m_indices;
}

void ShapeBuilder::setIndices(const BufferData<uint> &indices) {
    m_indices = indices;
}

Object* ShapeBuilder::createImpl() {
    createShapeIfNull();

    if (!m_modelPath.empty()) {
        loadFile();
    }

    loadShape();

    return m_shape;
}

static uint getMaxBonesPerVertex(const aiMesh *mesh) {
    auto bonesPerVertices = (uint*) calloc(mesh->mNumVertices, sizeof(uint));

    for (uint i = 0; i < mesh->mNumBones; i++) {
        const aiBone *bone = mesh->mBones[i];

        for (uint j = 0; j < bone->mNumWeights; j++) {
            const aiVertexWeight &weight = bone->mWeights[j];
            bonesPerVertices[weight.mVertexId]++;
        }
    }

    uint bonesPerVertex = 0; // max value from bonesPerVertices

    for (uint i = 0; i < mesh->mNumVertices; i++) {
        if (bonesPerVertex < bonesPerVertices[i]) {
            bonesPerVertex = bonesPerVertices[i];
        }
    }

    free(bonesPerVertices);

    return bonesPerVertex;
}

constexpr int PARAMS_TYPE_ASSIMP = 0;
constexpr int PARAMS_TYPE_ALGINE = 1;

template<int type>
inline static auto getParams(const vector<ShapeBuilder::Param> &params) {
    using Param = ShapeBuilder::Param;

    map<Param, uint> assimpParams = {
        {Param::Triangulate, aiProcess_Triangulate},
        {Param::SortByPolygonType, aiProcess_SortByPType},
        {Param::CalcTangentSpace, aiProcess_CalcTangentSpace},
        {Param::JoinIdenticalVertices, aiProcess_JoinIdenticalVertices}
    };

    vector<Param> algineParams;
    uint completeAssimpParams = 0;

    // sort params
    for (const auto p : params) {
        auto assimpParam = assimpParams.find(p);

        if (assimpParam != assimpParams.end()) {
            completeAssimpParams |= assimpParam->second;
        } else {
            algineParams.emplace_back(p);
        }
    }

    if constexpr (type == PARAMS_TYPE_ASSIMP) {
        return completeAssimpParams;
    } else {
        return algineParams;
    }
}

class AssimpCustomIOStream: public Assimp::IOStream {
public:
    explicit AssimpCustomIOStream(unique_ptr<algine::IOStream> &ioStream) {
        swap(m_ioStream, ioStream);
    }

    size_t Read(void *pvBuffer, size_t pSize, size_t pCount) override {
        return m_ioStream->read(pvBuffer, pSize, pCount);
    }

    size_t Write(const void *pvBuffer, size_t pSize, size_t pCount) override {
        return m_ioStream->write(pvBuffer, pSize, pCount);
    }

    aiReturn Seek(size_t pOffset, aiOrigin pOrigin) override {
        return static_cast<aiReturn>(m_ioStream->seek(pOffset, pOrigin));
    }

    size_t Tell() const override {
        return m_ioStream->tell();
    }

    size_t FileSize() const override {
        return m_ioStream->size();
    }

    void Flush() override {
        m_ioStream->flush();
    }

    void close() {
        m_ioStream->close();
    }

private:
    unique_ptr<algine::IOStream> m_ioStream;
};

class AssimpCustomIOSystem: public Assimp::IOSystem {
public:
    explicit AssimpCustomIOSystem(const shared_ptr<algine::IOSystem> &ioSystem)
        : m_ioSystem(ioSystem) {}

    bool Exists(const char *pFile) const override {
        return m_ioSystem->exists(pFile);
    }

    char getOsSeparator() const override {
        return m_ioSystem->getSeparator();
    }

    Assimp::IOStream *Open(const char *pFile, const char *pMode) override {
        auto isSame = [](const char *p1, const char *p2) { return !strcmp(p1, p2); };

        auto stream = m_ioSystem->open(pFile, [&]() {
            if (isSame(pMode, "r")) return IOStream::Mode::ReadText;
            if (isSame(pMode, "rb")) return IOStream::Mode::Read;
            if (isSame(pMode, "w")) return IOStream::Mode::WriteText;
            if (isSame(pMode, "wb")) return IOStream::Mode::Write;
            if (isSame(pMode, "a")) return IOStream::Mode::AppendText;
            if (isSame(pMode, "ab")) return IOStream::Mode::Append;

            return IOStream::Mode::None;
        }());

        return new AssimpCustomIOStream(stream);
    }

    void Close(Assimp::IOStream *pFile) override {
        static_cast<AssimpCustomIOStream *>(pFile)->close();
    }

private:
    shared_ptr<algine::IOSystem> m_ioSystem;
};

void ShapeBuilder::loadFile() {
    createShapeIfNull();

    // Create an instance of the Importer class
    Assimp::Importer importer;
    importer.SetIOHandler(new AssimpCustomIOSystem(io()));

    const aiScene *scene = importer.ReadFile(
            Path::join(getRootDir(), m_modelPath), algine::getParams<PARAMS_TYPE_ASSIMP>(m_params));

    // If the import failed, report it
    if (!scene) {
        Log::error(LOG_TAG) << "Assimp error: " << importer.GetErrorString();
        return;
    }

    m_shape->m_globalInverseTransform = glm::inverse(getMat4(scene->mRootNode->mTransformation));

    m_amtlManager.setParent(m_shape);

    // try to load AMTL from file if the current material is empty
    if (m_amtlManager.getMaterials().empty()) {
        string amtlPath = m_modelPath.substr(0, m_modelPath.find_last_of('.')) + ".amtl";

        // try to load AMTL
        if (string fullAMTLPath = Path::join(getRootDir(), amtlPath); io()->exists(Path(fullAMTLPath).toString())) {
            m_amtlManager.setIOSystem(io());
            m_amtlManager.loadFile(fullAMTLPath);
        }
    }

    // load shape
    processNode(scene->mRootNode, scene);

    // load animations
    m_shape->m_rootNode = Node(scene->mRootNode);
    m_shape->m_animations.reserve(scene->mNumAnimations); // allocate space for animations

    for (size_t i = 0; i < scene->mNumAnimations; i++) {
        m_shape->m_animations.emplace_back(scene->mAnimations[i]);
    }
}

void ShapeBuilder::loadShape() {
    applyParams();
    genBuffers();
    createInputLayouts();
}

void ShapeBuilder::applyParams() {
    createShapeIfNull();

    // apply algine params
    for (const auto p : algine::getParams<PARAMS_TYPE_ALGINE>(m_params)) {
        switch (p) {
            case Param::InverseNormals: {
                for (float &normal : m_normals.data) {
                    normal *= -1;
                }

                break;
            }
            case Param::DisableBones: {
                m_bonesPerVertex = 0;
                break;
            }
            default: {
                Log::error(LOG_TAG) << "Unknown algine param " << static_cast<uint>(p);
                break;
            }
        }
    }

    m_shape->m_bonesPerVertex = m_bonesPerVertex;
}

void ShapeBuilder::createInputLayouts() {
    createShapeIfNull();

    for (auto &item : m_locations) {
        m_shape->createInputLayout(item);
    }
}

Shape* ShapeBuilder::getCurrentShape() {
    return m_shape;
}

void ShapeBuilder::createShapeIfNull() {
    if (m_shape == nullptr) {
        m_shape = TypeRegistry::create<Shape>(alTypeRegistrySignatureTStr(m_className));
        m_shape->setParent(getActualParent());
    }
}

void ShapeBuilder::loadBones(const aiMesh *aimesh) {
    vector<BoneInfo> binfos; // bone infos
    binfos.reserve(aimesh->mNumVertices); // allocating space

    uint maxBonesPerVertex = getMaxBonesPerVertex(aimesh);

    if (maxBonesPerVertex < m_bonesPerVertex)
        maxBonesPerVertex = m_bonesPerVertex;

    // filling array
    for (size_t i = 0; i < aimesh->mNumVertices; i++)
        binfos.emplace_back(maxBonesPerVertex);

    // loading bone data
    for (usize i = 0; i < aimesh->mNumBones; i++) {
        aiBone *bone = aimesh->mBones[i];
        string boneName(bone->mName.data);
        Index boneIndex = m_shape->m_bones.getIndex(boneName);

        if (boneIndex == BonesStorage::BoneNotFound) {
            boneIndex = m_shape->m_bones.size();
            m_shape->m_bones.data().emplace_back(boneName, getMat4(bone->mOffsetMatrix));
        }

        for (usize j = 0; j < bone->mNumWeights; j++) {
            const aiVertexWeight &vertexWeight = bone->mWeights[j];
            binfos[vertexWeight.mVertexId].add(boneIndex, vertexWeight.mWeight);
        }
    }

    if (maxBonesPerVertex > m_bonesPerVertex) {
        for (auto & binfo : binfos) {
            binfo.sort(); // sorting by weights. From more to less

            // calculating weights sum for active bones
            float weightsSum = 0;
            for (uint j = 0; j < m_bonesPerVertex; j++)
                weightsSum += binfo.getWeight(j);

            // normalizing weights
            // weight_0 + weight_1 + ... + weight_j-1 = 1
            for (uint j = 0; j < m_bonesPerVertex; j++) {
                binfo.setWeight(j, binfo.getWeight(j) / weightsSum);
            }
        }
    }

    // converting to a suitable view
    for (const auto & binfo : binfos) {
        for (size_t j = 0; j < m_bonesPerVertex; j++) {
            m_boneIds->push_back(binfo.getId(j));
            m_boneWeights->push_back(binfo.getWeight(j));
        }
    }
}

void ShapeBuilder::processNode(const aiNode *node, const aiScene *scene) {
    // обработать все полигональные сетки в узле (если есть)
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        processMesh(mesh, scene);

        if (m_bonesPerVertex != 0) {
            loadBones(mesh);
        }
    }

    // выполнить ту же обработку и для каждого потомка узла
    for (size_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

void ShapeBuilder::processMesh(const aiMesh *aimesh, const aiScene *scene) {
    Mesh mesh;
    mesh.start = m_indices->size();
    uint verticesAtBeginning = m_vertices->size() / 3;

    // allocating space for vertices, normals, texCoords, tangents and bitangents
    m_vertices->reserve(aimesh->mNumVertices * 3);

    if (aimesh->HasNormals()) {
        m_normals->reserve(aimesh->mNumVertices * 3);
    } if (aimesh->HasTextureCoords(0)) {
        m_texCoords->reserve(aimesh->mNumVertices * 2);
    } if (aimesh->HasTangentsAndBitangents()) {
        m_tangents->reserve(aimesh->mNumVertices * 3);
        m_bitangents->reserve(aimesh->mNumVertices * 3);
    }

    for (size_t i = 0; i < aimesh->mNumVertices; i++) {
        // vertices
        m_vertices->push_back(aimesh->mVertices[i].x);
        m_vertices->push_back(aimesh->mVertices[i].y);
        m_vertices->push_back(aimesh->mVertices[i].z);

        // normals
        if (aimesh->HasNormals()) {
            m_normals->push_back(aimesh->mNormals[i].x);
            m_normals->push_back(aimesh->mNormals[i].y);
            m_normals->push_back(aimesh->mNormals[i].z);
        }

        // texCoords
        if (aimesh->HasTextureCoords(0)) {
            m_texCoords->push_back(aimesh->mTextureCoords[0][i].x);
            m_texCoords->push_back(aimesh->mTextureCoords[0][i].y);
        }

        // tangents and bitangents
        if (aimesh->HasTangentsAndBitangents()) {
            m_tangents->push_back(aimesh->mTangents[i].x);
            m_tangents->push_back(aimesh->mTangents[i].y);
            m_tangents->push_back(aimesh->mTangents[i].z);

            m_bitangents->push_back(aimesh->mBitangents[i].x);
            m_bitangents->push_back(aimesh->mBitangents[i].y);
            m_bitangents->push_back(aimesh->mBitangents[i].z);
        }
    }

    // faces
    for (size_t i = 0; i < aimesh->mNumFaces; i++) {
        for (size_t j = 0; j < aimesh->mFaces[i].mNumIndices; j++) {
            m_indices->push_back(aimesh->mFaces[i].mIndices[j] + verticesAtBeginning);
        }
    }

    mesh.count = m_indices->size() - mesh.start;

    // load classic & AMTL material
    aiMaterial *pAiMaterial = scene->mMaterials[aimesh->mMaterialIndex];

    auto &material = mesh.material;
    material.setName(pAiMaterial->GetName().C_Str());

    AMTLMaterialManager dummyAMTLMaterialManager;
    auto &amtlMaterialManager =
            m_amtlManager.isMaterialExists(material.getName()) ?
            m_amtlManager.getMaterial(material.getName()) : dummyAMTLMaterialManager;

    // I. Load textures from Algine Material
    for (const string &type : amtlMaterialManager.collectTextureNames()) {
        auto texture = amtlMaterialManager.loadTexture(type, m_shape);
        material.setTexture2D(type, texture);
    }

    // II. Load textures from Assimp Material
    auto assimpTextures = map<aiTextureType, const char*> {
            {aiTextureType_AMBIENT, Material::AmbientTexture},
            {aiTextureType_DIFFUSE, Material::DiffuseTexture},
            {aiTextureType_SPECULAR, Material::SpecularTexture},
            {aiTextureType_NORMALS, Material::NormalTexture}
    };

    for (auto &p : assimpTextures) {
        auto &texName = p.second;

        if (material.hasTexture2D(texName)) {
            continue; // texture has been already loaded from Algine Material
        }

        auto assimpType = p.first;

        if (assimpType == aiTextureType_UNKNOWN)
            continue;

        aiString path;
        pAiMaterial->GetTexture(assimpType, 0, &path);

        if (path.length == 0)
            continue;

        aiTextureMapMode mapModeU = aiTextureMapMode_Clamp;
        pAiMaterial->Get(AI_MATKEY_MAPPINGMODE_U(assimpType, 0), mapModeU);

        aiTextureMapMode mapModeV = aiTextureMapMode_Clamp;
        pAiMaterial->Get(AI_MATKEY_MAPPINGMODE_V(assimpType, 0), mapModeV);

        auto getMapMode = [](aiTextureMapMode assimpMode) {
            switch (assimpMode) {
                case aiTextureMapMode_Wrap: return Texture::Repeat;
                case aiTextureMapMode_Clamp: return Texture::ClampToEdge;
                case aiTextureMapMode_Decal: return Texture::ClampToBorder;
                case aiTextureMapMode_Mirror: return Texture::MirroredRepeat;
                default: return Texture::ClampToEdge;
            }
        };

        Texture2DBuilder builder;
        builder.setIOSystem(io());
        builder.setRootDir(Path(Path::join(getRootDir(), m_modelPath)).getParentDirectory().toString());
        builder.setPath(path.C_Str());
        builder.setParent(m_shape);
        builder.setParams({
            {Texture::WrapU, getMapMode(mapModeU)},
            {Texture::WrapV, getMapMode(mapModeV)},
            {Texture::MinFilter, Texture::Linear},
            {Texture::MagFilter, Texture::Linear}
        });

        material.setTexture2D(texName, builder.create());
    }

    // III. Set values
    material.setFloats(amtlMaterialManager.getFloats());

    if (!material.hasFloat(Material::Shininess)) {
        float shininess;
        pAiMaterial->Get(AI_MATKEY_SHININESS, shininess);
        material.setFloat(Material::Shininess, shininess);
    }

    // the result is undefined if shininess ≤ 0
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/pow.xhtml
    // FLT_EPSILON - min positive value for float
    if (material.getFloat(Material::Shininess) == 0)
        material.setFloat(Material::Shininess, FLT_EPSILON);

    m_shape->m_meshes.push_back(mesh);
}

// C++20 template lambda
template<typename BufferType, typename DataType>
inline BufferType* createBuffer(Object *parent, ShapeBuilder::BufferData<DataType> &data) {
    if (!data->empty()) {
        auto bufferType = new BufferType(parent);
        bufferType->bind();
        bufferType->setData(sizeof(DataType) * data->size(), data->data(), data.usage);
        bufferType->unbind();
        return bufferType;
    }

    return nullptr;
}

void ShapeBuilder::genBuffers() {
    createShapeIfNull();
    m_shape->m_vertices = createBuffer<ArrayBuffer>(m_shape, m_vertices);
    m_shape->m_normals = createBuffer<ArrayBuffer>(m_shape, m_normals);
    m_shape->m_texCoords = createBuffer<ArrayBuffer>(m_shape, m_texCoords);
    m_shape->m_tangents = createBuffer<ArrayBuffer>(m_shape, m_tangents);
    m_shape->m_bitangents = createBuffer<ArrayBuffer>(m_shape, m_bitangents);
    m_shape->m_boneWeights = createBuffer<ArrayBuffer>(m_shape, m_boneWeights);
    m_shape->m_boneIds = createBuffer<ArrayBuffer>(m_shape, m_boneIds);
    m_shape->m_indices = createBuffer<IndexBuffer>(m_shape, m_indices);
}

void ShapeBuilder::exec(const std::string &s, bool path, Lua *lua, sol::environment *tenv) {
    exec_t<ShapeBuilder>(s, path, lua, tenv);
}
}
