#define GLM_FORCE_CTOR_INIT
#include <algine/std/model/ShapeManager.h>

#include <algine/std/animation/BoneInfo.h>
#include <algine/core/texture/Texture2D.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <tulz/Path.h>

#include <iostream>
#include <cfloat>

#include "../assimp2glm.h"

using namespace tulz;
using namespace std;

namespace algine {
ShapeManager::ShapeManager() {
    m_shape = new Shape();
}

void ShapeManager::load() {
    map<Param, uint> assimpParams = {
        {Param::Triangulate, aiProcess_Triangulate},
        {Param::SortByPolygonType, aiProcess_SortByPType},
        {Param::CalcTangentSpace, aiProcess_CalcTangentSpace},
        {Param::JoinIdenticalVertices, aiProcess_JoinIdenticalVertices}
    };

    vector<Param> algineParams;
    uint completeAssimpParams = 0;

    // sort params
    for (const auto p : m_params) {
        auto assimpParam = assimpParams.find(p);

        if (assimpParam != assimpParams.end()) {
            completeAssimpParams |= assimpParam->second;
        } else {
            algineParams.emplace_back(p);
        }
    }

    // Create an instance of the Importer class
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(m_modelPath, completeAssimpParams);

    // If the import failed, report it
    if (!scene) {
        cerr << "Assimp error: " << importer.GetErrorString() << "\n";
        return;
    }

    m_shape->globalInverseTransform = glm::inverse(getMat4(scene->mRootNode->mTransformation));

    // try to find AMTL if does not specified
    if (m_amtlPath.empty())
        m_amtlPath = m_modelPath.substr(0, m_modelPath.find_last_of('.')) + ".amtl";

    // try to load AMTL
    if (Path(m_amtlPath).exists())
        m_amtlManager.importFromFile(m_amtlPath);

    // load shape
    processNode(scene->mRootNode, scene);

    // load animations
    m_shape->rootNode = Node(scene->mRootNode);
    m_shape->animations.reserve(scene->mNumAnimations); // allocate space for animations

    for (size_t i = 0; i < scene->mNumAnimations; i++)
        m_shape->animations.emplace_back(scene->mAnimations[i]);

    // apply algine params
    for (const auto p : algineParams) {
        switch (p) {
            case Param::InverseNormals:
                for (float &normal : normals) {
                    normal *= -1;
                }
                break;
            case Param::PrepareAllAnimations:
                for (auto & animation : m_shape->animations) {
                    animation.bones.resize(m_shape->bonesStorage.count());
                }
                break;
            case Param::DisableBones:
                m_bonesPerVertex = 0;
                break;
            default:
                cerr << "Unknown algine param " << static_cast<uint>(p) << "\n";
                break;
        }
    }

    m_shape->bonesPerVertex = m_bonesPerVertex;

    // generate buffers
    genBuffers();
}

void ShapeManager::addParam(Param param) {
    m_params.emplace_back(param);
}

void ShapeManager::addParams(const vector<Param> &params) {
    for (auto param : params) {
        m_params.emplace_back(param);
    }
}

void ShapeManager::setParams(const vector<Param> &params) {
    m_params = params;
}

void ShapeManager::setModelPath(const string &path) {
    m_modelPath = path;
}

void ShapeManager::setAMTLPath(const string &path) {
    m_amtlPath = path;
}

void ShapeManager::setBonesPerVertex(uint bonesPerVertex) {
    m_bonesPerVertex = bonesPerVertex;
}

const vector<ShapeManager::Param>& ShapeManager::getParams() const {
    return m_params;
}

const string& ShapeManager::getModelPath() const {
    return m_modelPath;
}

const string& ShapeManager::getAMTLPath() const {
    return m_amtlPath;
}

uint ShapeManager::getBonesPerVertex() const {
    return m_bonesPerVertex;
}

Shape *ShapeManager::getShape() const {
    return m_shape;
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

void ShapeManager::loadBones(const aiMesh *aimesh) {
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
            for (uint j = 0; j < m_bonesPerVertex; j++) {
                binfo.setWeight(j, binfo.getWeight(j) / weightsSum);
            }
        }
    }

    // converting to a suitable view
    for (const auto & binfo : binfos) {
        for (size_t j = 0; j < m_bonesPerVertex; j++) {
            boneIds.push_back(binfo.getId(j));
            boneWeights.push_back(binfo.getWeight(j));
        }
    }
}

void ShapeManager::processNode(const aiNode *node, const aiScene *scene) {
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

void ShapeManager::processMesh(const aiMesh *aimesh, const aiScene *scene) {
    Mesh mesh;
    mesh.start = indices.size();
    uint verticesAtBeginning = vertices.size() / 3;

    // allocating space for vertices, normals, texCoords, tangents and bitangents
    vertices.reserve(aimesh->mNumVertices * 3);

    if (aimesh->HasNormals()) {
        normals.reserve(aimesh->mNumVertices * 3);
    } if (aimesh->HasTextureCoords(0)) {
        texCoords.reserve(aimesh->mNumVertices * 2);
    } if (aimesh->HasTangentsAndBitangents()) {
        tangents.reserve(aimesh->mNumVertices * 3);
        bitangents.reserve(aimesh->mNumVertices * 3);
    }

    for (size_t i = 0; i < aimesh->mNumVertices; i++) {
        // vertices
        vertices.push_back(aimesh->mVertices[i].x);
        vertices.push_back(aimesh->mVertices[i].y);
        vertices.push_back(aimesh->mVertices[i].z);

        // normals
        if (aimesh->HasNormals()) {
            normals.push_back(aimesh->mNormals[i].x);
            normals.push_back(aimesh->mNormals[i].y);
            normals.push_back(aimesh->mNormals[i].z);
        }

        // texCoords
        if (aimesh->HasTextureCoords(0)) {
            texCoords.push_back(aimesh->mTextureCoords[0][i].x);
            texCoords.push_back(aimesh->mTextureCoords[0][i].y);
        }

        // tangents and bitangents
        if (aimesh->HasTangentsAndBitangents()) {
            tangents.push_back(aimesh->mTangents[i].x);
            tangents.push_back(aimesh->mTangents[i].y);
            tangents.push_back(aimesh->mTangents[i].z);

            bitangents.push_back(aimesh->mBitangents[i].x);
            bitangents.push_back(aimesh->mBitangents[i].y);
            bitangents.push_back(aimesh->mBitangents[i].z);
        }
    }

    // faces
    for (size_t i = 0; i < aimesh->mNumFaces; i++) {
        for (size_t j = 0; j < aimesh->mFaces[i].mNumIndices; j++) {
            indices.push_back(aimesh->mFaces[i].mIndices[j] + verticesAtBeginning);
        }
    }

    mesh.count = indices.size() - mesh.start;

    // load classic & AMTL material
    aiMaterial *material = scene->mMaterials[aimesh->mMaterialIndex];

    mesh.material.name = material->GetName().C_Str();

    AMTLMaterialManager dummyAMTLMaterialManager;
    auto &amtlMaterialManager =
            m_amtlManager.isMaterialExists(mesh.material.name) ?
            m_amtlManager.getMaterial(mesh.material.name) : dummyAMTLMaterialManager;

    using TextureType = AMTLMaterialManager::Texture;

    auto loadTexture = [&](TextureType type)
    {
        auto getAssimpType = [&]()
        {
            switch (type) {
                case TextureType::Ambient: return aiTextureType_AMBIENT;
                case TextureType::Diffuse: return aiTextureType_DIFFUSE;
                case TextureType::Specular: return aiTextureType_SPECULAR;
                case TextureType::Normal: return aiTextureType_NORMALS;
                default: return aiTextureType_UNKNOWN;
            }
        };

        auto getTexturePtr = [&]() -> auto&
        {
            switch (type) {
                case TextureType::Ambient: return mesh.material.ambientTexture;
                case TextureType::Diffuse: return mesh.material.diffuseTexture;
                case TextureType::Specular: return mesh.material.specularTexture;
                case TextureType::Normal: return mesh.material.normalTexture;
                case TextureType::Reflection: return mesh.material.reflectionTexture;
                case TextureType::Jitter: return mesh.material.jitterTexture;
                default: assert(0);
            }
        };

        // try to load texture from AMTLMaterial
        {
            auto &texture = getTexturePtr();

            texture = amtlMaterialManager.loadTexture(type);

            if (texture != nullptr) {
                return;
            }
        }

        // otherwise try to load texture from aiMaterial
        {
            auto assimpType = getAssimpType();

            if (assimpType == aiTextureType_UNKNOWN)
                return;

            aiString path;
            material->GetTexture(assimpType, 0, &path);

            aiTextureMapMode mapModeU = aiTextureMapMode_Clamp;
            material->Get(AI_MATKEY_MAPPINGMODE_U(assimpType, 0), mapModeU);

            aiTextureMapMode mapModeV = aiTextureMapMode_Clamp;
            material->Get(AI_MATKEY_MAPPINGMODE_V(assimpType, 0), mapModeV);

            auto getMapMode = [](aiTextureMapMode assimpMode)
            {
                switch (assimpMode) {
                    case aiTextureMapMode_Wrap: return Texture::Repeat;
                    case aiTextureMapMode_Clamp: return Texture::ClampToEdge;
                    case aiTextureMapMode_Decal: return Texture::ClampToBorder;
                    case aiTextureMapMode_Mirror: return Texture::MirroredRepeat;
                    default: return Texture::ClampToEdge;
                }
            };

            Texture2DManager manager;
            manager.setWorkingDirectory(Path(m_modelPath).getParentDirectory());
            manager.setPath(path.C_Str());
            manager.setParams({
                {Texture::WrapU, getMapMode(mapModeU)},
                {Texture::WrapV, getMapMode(mapModeV)},
                {Texture::MinFilter, Texture::Linear},
                {Texture::MagFilter, Texture::Linear}
            });

            getTexturePtr() = manager.create();
        }
    };

    loadTexture(TextureType::Ambient);
    loadTexture(TextureType::Diffuse);
    loadTexture(TextureType::Specular);
    loadTexture(TextureType::Normal);
    loadTexture(TextureType::Reflection);
    loadTexture(TextureType::Jitter);

    mesh.material.ambientStrength = amtlMaterialManager.getAmbientStrength();
    mesh.material.diffuseStrength = amtlMaterialManager.getDiffuseStrength();
    mesh.material.specularStrength = amtlMaterialManager.getSpecularStrength();
    mesh.material.reflection = amtlMaterialManager.getReflection();
    mesh.material.jitter = amtlMaterialManager.getJitter();

    if (amtlMaterialManager.getShininess() != -1) {
        mesh.material.shininess = amtlMaterialManager.getShininess();
    } else {
        material->Get(AI_MATKEY_SHININESS, mesh.material.shininess);
    }

    // the result is undefined if shininess ≤ 0
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/pow.xhtml
    // FLT_EPSILON - min positive value for float
    if (mesh.material.shininess == 0)
        mesh.material.shininess = FLT_EPSILON;

    m_shape->meshes.push_back(mesh);
}

template<typename BufferType, typename DataType>
inline BufferType* createBuffer(const vector<DataType> &data) {
    if (!data.empty()) {
        auto *bufferType = new BufferType();
        bufferType->bind();
        bufferType->setData(sizeof(data[0]) * data.size(), &data[0], Buffer::StaticDraw);
        bufferType->unbind();
        return bufferType;
    }

    return nullptr;
}

void ShapeManager::genBuffers() {
    m_shape->buffers.vertices = createBuffer<ArrayBuffer>(vertices);
    m_shape->buffers.normals = createBuffer<ArrayBuffer>(normals);
    m_shape->buffers.texCoords = createBuffer<ArrayBuffer>(texCoords);
    m_shape->buffers.tangents = createBuffer<ArrayBuffer>(tangents);
    m_shape->buffers.bitangents = createBuffer<ArrayBuffer>(bitangents);
    m_shape->buffers.boneWeights = createBuffer<ArrayBuffer>(boneWeights);
    m_shape->buffers.boneIds = createBuffer<ArrayBuffer>(boneIds);

    m_shape->buffers.indices = createBuffer<IndexBuffer>(indices);
}
}
