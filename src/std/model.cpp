#define GLM_FORCE_CTOR_INIT
#include <algine/std/model.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <algine/std/Node.h>
#include <algine/std/animation/BoneInfo.h>
#include <tulz/Path>

using namespace tulz;
using namespace std;

namespace algine {
void Shape::delBuffers() {
    for (auto &item : inputLayouts)
        InputLayout::destroy(item);

    ArrayBuffer::destroy(buffers.vertices, buffers.normals, buffers.texCoords,
            buffers.tangents, buffers.bitangents, buffers.boneWeights, buffers.boneIds);
    IndexBuffer::destroy(buffers.indices);
}

inline void
addAttribute(
        const InputLayout *inputLayout,
        const InputAttributeDescription &inputAttributeDescription,
        const ArrayBuffer *arrayBuffer)
{
    if (inputAttributeDescription.m_location != InputAttributeDescription::LocationAbsent && arrayBuffer != nullptr)
        inputLayout->addAttribute(inputAttributeDescription, arrayBuffer);
}

void Shape::createInputLayout(
        int inPosition, int inTexCoord, int inNormal,
        int inTangent, int inBitangent,
        int inBoneWeights, int inBoneIds
    ) {
    auto *inputLayout = new InputLayout();
    inputLayout->bind();
    inputLayouts.push_back(inputLayout);

    InputAttributeDescription attribDescription;
    attribDescription.setCount(3);

    attribDescription.setLocation(inPosition);
    addAttribute(inputLayout, attribDescription, buffers.vertices);

    attribDescription.setLocation(inNormal);
    addAttribute(inputLayout, attribDescription, buffers.normals);

    attribDescription.setLocation(inTangent);
    addAttribute(inputLayout, attribDescription, buffers.tangents);

    attribDescription.setLocation(inBitangent);
    addAttribute(inputLayout, attribDescription, buffers.bitangents);

    attribDescription.setLocation(inTexCoord);
    attribDescription.setCount(2);
    addAttribute(inputLayout, attribDescription, buffers.texCoords);

    if (bonesPerVertex != 0) {
        attribDescription.setCount(4);

        attribDescription.setLocation(inBoneWeights);
        addAttribute(inputLayout, attribDescription, buffers.boneWeights);

        attribDescription.setLocation(inBoneIds);
        attribDescription.setFormat(GL_UNSIGNED_INT); // TODO
        addAttribute(inputLayout, attribDescription, buffers.boneIds);
    }

    inputLayout->setIndexBuffer(buffers.indices);
    inputLayout->unbind();
}

void Shape::setNodeTransform(const std::string &nodeName, const glm::mat4 &transformation) {
    rootNode.getNode(nodeName)->transformation = transformation;
}

void Shape::recycle() {
    delBuffers();
}

Model::Model(const uint rotatorType): rotatable(rotatorType) {
    /* empty */
}

void Model::updateMatrix() {
    m_transform = m_translation * m_rotation * m_scaling;
}

inline int getBoneIndex(const Shape *shape, const std::string &name) {
    for (usize i = 0; i < shape->bones.size(); i++)
        if (shape->bones[i].name == name)
            return i;

    return -1;
}

void ShapeLoader::loadBones(const aiMesh *aimesh) {
    std::vector<BoneInfo> binfos; // bone infos
    binfos.reserve(aimesh->mNumVertices); // allocating space

    // filling array
    for (size_t i = 0; i < aimesh->mNumVertices; i++)
        binfos.emplace_back(m_shape->bonesPerVertex);

    // loading bone data
    for (usize i = 0; i < aimesh->mNumBones; i++) {
        aiBone *bone = aimesh->mBones[i];
        std::string boneName(bone->mName.data);
        int boneIndex = getBoneIndex(m_shape, boneName);

        if (boneIndex == -1) {
            boneIndex = m_shape->bones.size();
            m_shape->bones.emplace_back(boneName, getMat4(bone->mOffsetMatrix));
        }

        for (usize j = 0; j < bone->mNumWeights; j++) {
            aiVertexWeight *vertexWeight = &(bone->mWeights[j]);
            binfos[vertexWeight->mVertexId].add(boneIndex, vertexWeight->mWeight);
        }
    }

    // converting to a suitable view
    for (size_t i = 0; i < binfos.size(); i++) {
        for (size_t j = 0; j < binfos[i].size(); j++) {
            m_shape->geometry.boneIds.push_back(binfos[i].ids[j]);
            m_shape->geometry.boneWeights.push_back(binfos[i].weights[j]);
        }
    }
}

void ShapeLoader::processNode(const aiNode *node, const aiScene *scene) {
    // обработать все полигональные сетки в узле (если есть)
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);
        if (m_shape->bonesPerVertex != 0)
            loadBones(mesh);
    }

    // выполнить ту же обработку и для каждого потомка узла
    for (size_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

inline void mergeTexturePath(std::string &saveTo, AMTLLoader::MaterialObject &amtlMaterialObject, const uint key) {
    if (amtlMaterialObject.textures.find(key) != amtlMaterialObject.textures.end() &&
        !amtlMaterialObject.textures[key].path.empty())
    {
        saveTo = amtlMaterialObject.textures[key].path;
    }
}

void ShapeLoader::processMesh(const aiMesh *aimesh, const aiScene *scene) {
    Mesh mesh;
    mesh.start = m_shape->geometry.indices.size();
    uint verticesAtBeggining = m_shape->geometry.vertices.size() / 3;

    // allocating space for vertices, normals, texCoords, tangents and bitangents
    m_shape->geometry.vertices.reserve(aimesh->mNumVertices * 3);
    if (aimesh->HasNormals())
        m_shape->geometry.normals.reserve(aimesh->mNumVertices * 3);
    if (aimesh->HasTextureCoords(0))
        m_shape->geometry.texCoords.reserve(aimesh->mNumVertices * 2);
    if (aimesh->HasTangentsAndBitangents()) {
        m_shape->geometry.tangents.reserve(aimesh->mNumVertices * 3);
        m_shape->geometry.bitangents.reserve(aimesh->mNumVertices * 3);
    }

    for (size_t i = 0; i < aimesh->mNumVertices; i++) {
        // vertices
        m_shape->geometry.vertices.push_back(aimesh->mVertices[i].x);
        m_shape->geometry.vertices.push_back(aimesh->mVertices[i].y);
        m_shape->geometry.vertices.push_back(aimesh->mVertices[i].z);

        // normals
        if (aimesh->HasNormals()) {
            m_shape->geometry.normals.push_back(aimesh->mNormals[i].x);
            m_shape->geometry.normals.push_back(aimesh->mNormals[i].y);
            m_shape->geometry.normals.push_back(aimesh->mNormals[i].z);
        }

        // texCoords
        if (aimesh->HasTextureCoords(0)) {
            m_shape->geometry.texCoords.push_back(aimesh->mTextureCoords[0][i].x);
            m_shape->geometry.texCoords.push_back(aimesh->mTextureCoords[0][i].y);
        }

        // tangents and bitangents
        if (aimesh->HasTangentsAndBitangents()) {
            m_shape->geometry.tangents.push_back(aimesh->mTangents[i].x);
            m_shape->geometry.tangents.push_back(aimesh->mTangents[i].y);
            m_shape->geometry.tangents.push_back(aimesh->mTangents[i].z);

            m_shape->geometry.bitangents.push_back(aimesh->mBitangents[i].x);
            m_shape->geometry.bitangents.push_back(aimesh->mBitangents[i].y);
            m_shape->geometry.bitangents.push_back(aimesh->mBitangents[i].z);
        }
    }

    // faces
    for (size_t i = 0; i < aimesh->mNumFaces; i++) {
        for (size_t j = 0; j < aimesh->mFaces[i].mNumIndices; j++) {
            m_shape->geometry.indices.push_back(aimesh->mFaces[i].mIndices[j] + verticesAtBeggining);
        }
    }

    mesh.count = m_shape->geometry.indices.size() - mesh.start;

    // load classic material & merge with AMTL
    aiString tmp_str;
    aiMaterial *material = scene->mMaterials[aimesh->mMaterialIndex];

    mesh.material.name = material->GetName().C_Str();

    MaterialTexPaths materialTexPaths;

    material->GetTexture(aiTextureType_AMBIENT, 0, &tmp_str);
    materialTexPaths.ambient = tmp_str.C_Str();

    material->GetTexture(aiTextureType_DIFFUSE, 0, &tmp_str);
    materialTexPaths.diffuse = tmp_str.C_Str();

    material->GetTexture(aiTextureType_SPECULAR, 0, &tmp_str);
    materialTexPaths.specular = tmp_str.C_Str();

    material->GetTexture(aiTextureType_NORMALS, 0, &tmp_str);
    materialTexPaths.normal = tmp_str.C_Str();

    material->Get(AI_MATKEY_SHININESS, mesh.material.shininess);

    if (m_amtlLoader) {
        tmp_str = material->GetName();
        if (m_amtlLoader->m_materials.find(tmp_str.C_Str()) != m_amtlLoader->m_materials.end()) {
            AMTLLoader::MaterialObject amtlMaterial = m_amtlLoader->m_materials[tmp_str.C_Str()];

            // merge classic material with AMTL:
            //  * some data is written immediately to the final material (such as *Strength, reflection, jitter, shininess);
            //  * some (such as texture path) are merged and written to a separate array;
            //  * some are used from AMTLLoader (such as texture parameters and their shared level);
            // AMTL has the highest priority, so if the value is in AMTL,
            // then it is used, and the value of the classical material is ignored
            // Note: classic material means the one that comes with the model by default
            mergeTexturePath(materialTexPaths.ambient, amtlMaterial, AMTLLoader::AmbientTexture);
            mergeTexturePath(materialTexPaths.diffuse, amtlMaterial, AMTLLoader::DiffuseTexture);
            mergeTexturePath(materialTexPaths.specular, amtlMaterial, AMTLLoader::SpecularTexture);
            mergeTexturePath(materialTexPaths.normal, amtlMaterial, AMTLLoader::NormalTexture);
            mergeTexturePath(materialTexPaths.reflection, amtlMaterial, AMTLLoader::ReflectionTexture);
            mergeTexturePath(materialTexPaths.jitter, amtlMaterial, AMTLLoader::JitterTexture);
            mesh.material.ambientStrength = amtlMaterial.ambientStrength;
            mesh.material.diffuseStrength = amtlMaterial.diffuseStrength;
            mesh.material.specularStrength = amtlMaterial.specularStrength;
            mesh.material.reflection = amtlMaterial.reflection;
            mesh.material.jitter = amtlMaterial.jitter;
            if (amtlMaterial.shininess != -1)
                mesh.material.shininess = amtlMaterial.shininess;
        }
    }

    // the result is undefined if shininess ≤ 0
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/pow.xhtml
    // FLT_EPSILON - min positive value for float
    if (mesh.material.shininess == 0)
        mesh.material.shininess = FLT_EPSILON;

    m_materialTexPaths.push_back(materialTexPaths);
    m_shape->meshes.push_back(mesh);
}

#define textureTypesCount 6
void ShapeLoader::loadTextures() {
    if (m_texturesPath.empty())
        m_texturesPath = tulz::Path(m_modelPath).getParentDirectory();

    for (size_t i = 0; i < m_shape->meshes.size(); i++) {
        Material &material = m_shape->meshes[i].material;
        MaterialTexPaths &texPaths = m_materialTexPaths[i];

        // arrays for 100% certainty that nothing will go wrong
        // http://www.cplusplus.com/forum/general/267546/#msg1151147
        string *const texPathsArr[textureTypesCount] = {
                &texPaths.ambient, &texPaths.diffuse, &texPaths.specular,
                &texPaths.normal, &texPaths.reflection, &texPaths.jitter
        };

        shared_ptr<Texture2D> *const textures[textureTypesCount] = {
                &material.ambientTexture, &material.diffuseTexture, &material.specularTexture,
                &material.normalTexture, &material.reflectionTexture, &material.jitterTexture
        };

        // texIndex from AMTLLoader::AmbientTexture to AMTLLoader::JitterTexture
        for (uint texIndex = 0; texIndex < textureTypesCount; ++texIndex) {
            if (!texPathsArr[texIndex]->empty()) {
                string absolutePath = Path(*texPathsArr[texIndex]).isAbsolute() ?
                        *texPathsArr[texIndex] : Path::join(m_texturesPath, *texPathsArr[texIndex]);
                auto &currentTexture = *textures[texIndex];

                map<uint, uint> params = m_defaultTexturesParams;
                uint sharedLevel = AMTLLoader::Shared;

                if (m_amtlLoader != nullptr &&
                    m_amtlLoader->m_materials.find(material.name) != m_amtlLoader->m_materials.end())
                {
                    AMTLLoader::MaterialObject &materialObject = m_amtlLoader->m_materials[material.name];
                    if (materialObject.textures.find(texIndex) != materialObject.textures.end()) {
                        auto textureObject = materialObject.textures[texIndex];

                        sharedLevel = textureObject.sharedLevel;

                        // merging params: default + AMTL
                        for (const auto &key : textureObject.params)
                            params[key.first] = key.second;
                    }
                }

                // loading texture
                vector<LoadedTexture> *loadedTextures = nullptr;
                switch (sharedLevel) {
                    case AMTLLoader::Shared:
                        loadedTextures = &m_globalLoadedTextures;
                        break;
                    case AMTLLoader::ModelShared:
                        loadedTextures = &m_modelLoadedTextures;
                        break;
                    default:
                        break;
                }

                int index = getLoadedTextureIndex(loadedTextures, absolutePath, params);
                if (index != -1) {
                    currentTexture = loadedTextures->operator[](index).texture; // texture already loaded
                } else {
                    shared_ptr<Texture2D> texture2D = make_shared<Texture2D>();
                    texture2D->bind();
                    texture2D->fromFile(absolutePath);
                    texture2D->setParams(params);
                    texture2D->unbind();
                    currentTexture = texture2D;

                    if (sharedLevel != AMTLLoader::Unique)
                        loadedTextures->emplace_back(absolutePath, texture2D, params);
                }
            }
        }
    }
}

template<typename BufferType, typename DataType>
inline BufferType* createBuffer(const std::vector<DataType> &data) {
    if (!data.empty()) {
        auto *bufferType = new BufferType();
        bufferType->bind();
        bufferType->setData(sizeof(data[0]) * data.size(), &data[0], Buffer::StaticDraw);
        bufferType->unbind();
        return bufferType;
    }

    return nullptr;
}

void ShapeLoader::genBuffers() {
    m_shape->buffers.vertices = createBuffer<ArrayBuffer>(m_shape->geometry.vertices);
    m_shape->buffers.normals = createBuffer<ArrayBuffer>(m_shape->geometry.normals);
    m_shape->buffers.texCoords = createBuffer<ArrayBuffer>(m_shape->geometry.texCoords);
    m_shape->buffers.tangents = createBuffer<ArrayBuffer>(m_shape->geometry.tangents);
    m_shape->buffers.bitangents = createBuffer<ArrayBuffer>(m_shape->geometry.bitangents);
    m_shape->buffers.boneWeights = createBuffer<ArrayBuffer>(m_shape->geometry.boneWeights);
    m_shape->buffers.boneIds = createBuffer<ArrayBuffer>(m_shape->geometry.boneIds);

    m_shape->buffers.indices = createBuffer<IndexBuffer>(m_shape->geometry.indices);
}

ShapeLoader::LoadedTexture::LoadedTexture(
        const string &path, const shared_ptr<Texture2D> &texture,
        const map<uint, uint> &params)
{
    this->path = path;
    this->texture = texture;
    this->params = params;
}

// using vector instead of map since the same texture may have different params
vector<ShapeLoader::LoadedTexture> ShapeLoader::m_globalLoadedTextures;

int ShapeLoader::getLoadedTextureIndex(
        const vector<LoadedTexture> *const loadedTextures, const string &path,
        const map<uint, uint> &params)
{
    if (!loadedTextures)
        return -1;

    for (int i = 0; i < loadedTextures->size(); ++i)
        if (loadedTextures->operator[](i).path == path && loadedTextures->operator[](i).params == params)
            return i;

    return -1;
}

ShapeLoader::ShapeLoader() {
    m_shape = new Shape();
}

#define ASSIMP_PARAMS_MAX_INDEX JoinIdenticalVertices
#define isAssimpParam(param) param <= ASSIMP_PARAMS_MAX_INDEX
void ShapeLoader::load() {
    uint assimpParams[] = {
            aiProcess_Triangulate,
            aiProcess_SortByPType,
            aiProcess_CalcTangentSpace,
            aiProcess_JoinIdenticalVertices
    }, completeAssimpParams = 0;

    vector<int> algineParams;

    for (const uint p : m_params)
        if (isAssimpParam(p))
            completeAssimpParams |= assimpParams[p];
        else
            algineParams.emplace_back(p);

    // Create an instance of the Importer class
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(m_modelPath, completeAssimpParams);

    // If the import failed, report it
    if (!scene) {
        std::cerr << "Assimp error: " << importer.GetErrorString() << "\n";
        return;
    }

    m_shape->globalInverseTransform = getMat4(scene->mRootNode->mTransformation);
    m_shape->globalInverseTransform = glm::inverse(m_shape->globalInverseTransform);

    m_shape->rootNode = Node(scene->mRootNode);
    m_shape->animations.reserve(scene->mNumAnimations); // allocate space for animations
    for (size_t i = 0; i < scene->mNumAnimations; i++)
        m_shape->animations.emplace_back(scene->mAnimations[i]);

    std::string amtlPath = m_modelPath.substr(0, m_modelPath.find_last_of('.')) + ".amtl";
    AMTLLoader amtl;
    if (amtl.load(amtlPath))
       m_amtlLoader = &amtl;
    else {
        // if load is called again and the AMTL file does not exist,
        // while the previous load() AMTL existed, the pointer will
        // point to the deleted memory
        m_amtlLoader = nullptr;
    }

    processNode(scene->mRootNode, scene);

    // apply algine params
    for (const uint p : algineParams) {
        switch (p) {
            case InverseNormals:
                for (float &normal : m_shape->geometry.normals)
                    normal *= -1;
                break;
            default:
                std::cerr << "Unknown algine param " << p << "\n";
                break;
        }
    }

    // load textures
    loadTextures();

    // generate buffers
    genBuffers();
}

void ShapeLoader::addParam(uint param) {
    m_params.push_back(param);
}

void ShapeLoader::setModelPath(const std::string &path) {
    m_modelPath = path;
}

void ShapeLoader::setTexturesPath(const std::string &path) {
    m_texturesPath = path;
}

void ShapeLoader::setDefaultTexturesParams(const std::map<uint, uint> &params) {
    m_defaultTexturesParams = params;
}

Shape *ShapeLoader::getShape() const {
    return m_shape;
}
} // namespace algine
