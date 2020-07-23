#define GLM_FORCE_CTOR_INIT
#include <algine/std/model/ShapeLoader.h>

#include <algine/std/AMTLLoader.h>
#include <algine/std/animation/BoneInfo.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <tulz/Path.h>

#include <iostream>
#include <vector>
#include <cfloat>

#include "../assimp2glm.h"

using namespace tulz;
using namespace std;

namespace algine {
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

    m_shape->globalInverseTransform = glm::inverse(getMat4(scene->mRootNode->mTransformation));

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

    // load animations
    m_shape->rootNode = Node(scene->mRootNode);
    m_shape->animations.reserve(scene->mNumAnimations); // allocate space for animations
    for (size_t i = 0; i < scene->mNumAnimations; i++)
        m_shape->animations.emplace_back(scene->mAnimations[i]);

    // apply algine params
    for (const uint p : algineParams) {
        switch (p) {
            case InverseNormals:
                for (float &normal : geometry.normals)
                    normal *= -1;
                break;
            case PrepareAllAnimations:
                for (auto & animation : m_shape->animations)
                    animation.bones.resize(m_shape->bonesStorage.count());
                break;
            case DisableBones:
                m_bonesPerVertex = 0;
                break;
            default:
                std::cerr << "Unknown algine param " << p << "\n";
                break;
        }
    }

    m_shape->bonesPerVertex = m_bonesPerVertex;

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

void ShapeLoader::setBonesPerVertex(uint bonesPerVertex) {
    m_bonesPerVertex = bonesPerVertex;
}

Shape *ShapeLoader::getShape() const {
    return m_shape;
}

ShapeLoader::LoadedTexture::LoadedTexture(
        const string &path, const shared_ptr<Texture2D> &texture,
        const map<uint, uint> &params)
{
    this->path = path;
    this->texture = texture;
    this->params = params;
}

uint getMaxBonesPerVertex(const aiMesh *mesh) {
    uint *bonesPerVertices = (uint*) calloc(mesh->mNumVertices, sizeof(uint));

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

void ShapeLoader::loadBones(const aiMesh *aimesh) {
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
        int boneIndex = m_shape->bonesStorage.getIndex(boneName);

        if (boneIndex == BonesStorage::BoneNotFound) {
            boneIndex = m_shape->bonesStorage.count();
            m_shape->bonesStorage.bones.emplace_back(boneName, getMat4(bone->mOffsetMatrix));
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
            for (uint j = 0; j < m_bonesPerVertex; j++)
                binfo.setWeight(j, binfo.getWeight(j) / weightsSum);
        }
    }

    // converting to a suitable view
    for (auto & binfo : binfos) {
        for (size_t j = 0; j < m_bonesPerVertex; j++) {
            geometry.boneIds.push_back(binfo.getId(j));
            geometry.boneWeights.push_back(binfo.getWeight(j));
        }
    }
}

void ShapeLoader::processNode(const aiNode *node, const aiScene *scene) {
    // обработать все полигональные сетки в узле (если есть)
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh, scene);

        if (m_bonesPerVertex != 0)
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
    mesh.start = geometry.indices.size();
    uint verticesAtBeggining = geometry.vertices.size() / 3;

    // allocating space for vertices, normals, texCoords, tangents and bitangents
    geometry.vertices.reserve(aimesh->mNumVertices * 3);
    if (aimesh->HasNormals())
        geometry.normals.reserve(aimesh->mNumVertices * 3);
    if (aimesh->HasTextureCoords(0))
        geometry.texCoords.reserve(aimesh->mNumVertices * 2);
    if (aimesh->HasTangentsAndBitangents()) {
        geometry.tangents.reserve(aimesh->mNumVertices * 3);
        geometry.bitangents.reserve(aimesh->mNumVertices * 3);
    }

    for (size_t i = 0; i < aimesh->mNumVertices; i++) {
        // vertices
        geometry.vertices.push_back(aimesh->mVertices[i].x);
        geometry.vertices.push_back(aimesh->mVertices[i].y);
        geometry.vertices.push_back(aimesh->mVertices[i].z);

        // normals
        if (aimesh->HasNormals()) {
            geometry.normals.push_back(aimesh->mNormals[i].x);
            geometry.normals.push_back(aimesh->mNormals[i].y);
            geometry.normals.push_back(aimesh->mNormals[i].z);
        }

        // texCoords
        if (aimesh->HasTextureCoords(0)) {
            geometry.texCoords.push_back(aimesh->mTextureCoords[0][i].x);
            geometry.texCoords.push_back(aimesh->mTextureCoords[0][i].y);
        }

        // tangents and bitangents
        if (aimesh->HasTangentsAndBitangents()) {
            geometry.tangents.push_back(aimesh->mTangents[i].x);
            geometry.tangents.push_back(aimesh->mTangents[i].y);
            geometry.tangents.push_back(aimesh->mTangents[i].z);

            geometry.bitangents.push_back(aimesh->mBitangents[i].x);
            geometry.bitangents.push_back(aimesh->mBitangents[i].y);
            geometry.bitangents.push_back(aimesh->mBitangents[i].z);
        }
    }

    // faces
    for (size_t i = 0; i < aimesh->mNumFaces; i++) {
        for (size_t j = 0; j < aimesh->mFaces[i].mNumIndices; j++) {
            geometry.indices.push_back(aimesh->mFaces[i].mIndices[j] + verticesAtBeggining);
        }
    }

    mesh.count = geometry.indices.size() - mesh.start;

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
    m_shape->buffers.vertices = createBuffer<ArrayBuffer>(geometry.vertices);
    m_shape->buffers.normals = createBuffer<ArrayBuffer>(geometry.normals);
    m_shape->buffers.texCoords = createBuffer<ArrayBuffer>(geometry.texCoords);
    m_shape->buffers.tangents = createBuffer<ArrayBuffer>(geometry.tangents);
    m_shape->buffers.bitangents = createBuffer<ArrayBuffer>(geometry.bitangents);
    m_shape->buffers.boneWeights = createBuffer<ArrayBuffer>(geometry.boneWeights);
    m_shape->buffers.boneIds = createBuffer<ArrayBuffer>(geometry.boneIds);

    m_shape->buffers.indices = createBuffer<IndexBuffer>(geometry.indices);
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
}