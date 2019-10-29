#define GLM_FORCE_CTOR_INIT
#include <algine/model.h>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

#include <assimp/Importer.hpp>  // C++ importer interface

#include <algine/texture.h>
#include <algine/node.h>
#include <algine/algine_renderer.h>
#include <tulz/Path>

using namespace tulz;

namespace algine {
void Mesh::delMaterial() {
    mat.recycle();
}

void Mesh::recycle() {
    delMaterial();
}

Mesh Mesh::processMesh(const aiMesh *aimesh, const aiScene *scene, Geometry &geometry, AlgineMTL *amtl) {
    Mesh mesh;
    mesh.start = geometry.indices.size();
    uint verticesAtBeggining = geometry.vertices.size() / 3;
    
    // allocating space for vertices, normals, texCoords, tangents and bitangents
    geometry.vertices.reserve(aimesh->mNumVertices * 3);
    if (aimesh->HasNormals()) geometry.normals.reserve(aimesh->mNumVertices * 3);
    if (aimesh->HasTextureCoords(0)) geometry.texCoords.reserve(aimesh->mNumVertices * 2);
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

    // material
    aiString tmp_str;
    aiMaterial *aimat = scene->mMaterials[aimesh->mMaterialIndex];

    mesh.mat.name = aimat->GetName().C_Str();

    aimat->GetTexture(aiTextureType_AMBIENT, 0, &tmp_str);
    mesh.mat.texAmbientPath = tmp_str.C_Str();

    aimat->GetTexture(aiTextureType_DIFFUSE, 0, &tmp_str);
    mesh.mat.texDiffusePath = tmp_str.C_Str();

    aimat->GetTexture(aiTextureType_SPECULAR, 0, &tmp_str);
    mesh.mat.texSpecularPath = tmp_str.C_Str();

    aimat->GetTexture(aiTextureType_NORMALS, 0, &tmp_str);
    mesh.mat.texNormalPath = tmp_str.C_Str();

    aimat->Get(AI_MATKEY_SHININESS, mesh.mat.shininess);

    if (amtl != nullptr) {
        tmp_str = aimat->GetName();
        int index = amtl->forName(tmp_str.C_Str());
        if (index >= 0) {
            AlgineMT amt = amtl->materials[index];

            // algine specific material params
            mesh.mat.texReflectionPath = amt.texReflectionPath;
            mesh.mat.texJitterPath = amt.texJitterPath;
            mesh.mat.ambientStrength = amt.ambientStrength;
            mesh.mat.diffuseStrength = amt.diffuseStrength;
            mesh.mat.specularStrength = amt.specularStrength;
            mesh.mat.reflection = amt.reflection;
            mesh.mat.jitter = amt.jitter;

            // material params
            // sine - set if not empty
            #define sine(name) \
                if (!amt.name.empty()) mesh.mat.name = amt.name
            sine(texAmbientPath);
            sine(texDiffusePath);
            sine(texSpecularPath);
            sine(texNormalPath);
            if (amt.shininess != -1)
                mesh.mat.shininess = amt.shininess;
            #undef sine
        }
    }

    // the result is undefined if shininess ≤ 0
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/pow.xhtml
    // FLT_EPSILON - min positive value for float
    if (mesh.mat.shininess == 0) mesh.mat.shininess = FLT_EPSILON;

    return mesh;
}

uint Shape::loadTex(const std::string &fullpath, const std::string &path) {
    std::string absolutePath = Path(path).isAbsolute() ? path : Path::join(fullpath, path);
    
    if (loadedTextures.find(absolutePath) != loadedTextures.end())
        return loadedTextures[absolutePath]; // texture already loaded
    else {
        GLuint id = loadTexture(absolutePath.c_str());
        loadedTextures[absolutePath] = id;
        return id;
    }
}

size Shape::getBoneIndex(const std::string &name) {
    for (usize i = 0; i < bones.size(); i++)
        if (bones[i].name == name)
            return i;

    return -1;
}

void Shape::loadBones(const aiMesh *aimesh) {
    std::vector<BoneInfo> binfos; // bone infos
    binfos.reserve(aimesh->mNumVertices); // allocating space
    
    // filling array
    for (size_t i = 0; i < aimesh->mNumVertices; i++)
        binfos.emplace_back(bonesPerVertex);

    // loading bone data
    aiBone *bone;
    aiVertexWeight *vertexWeight;
    size boneIndex;
    for (usize i = 0; i < aimesh->mNumBones; i++) {
        bone = aimesh->mBones[i];
        std::string boneName(bone->mName.data);

        if ((boneIndex = getBoneIndex(boneName)) == -1) {
            boneIndex = bones.size();
            bones.emplace_back(boneName, getMat4(bone->mOffsetMatrix));
        }

        for (usize j = 0; j < bone->mNumWeights; j++) {
            vertexWeight = &(bone->mWeights[j]);
            binfos[vertexWeight->mVertexId].add(boneIndex, vertexWeight->mWeight);
        }
    }

    // converting to a suitable view
    for (size_t i = 0; i < binfos.size(); i++) {
        for (size_t j = 0; j < binfos[i].size; j++) {
            geometry.boneIds.push_back(binfos[i].ids[j]);
            geometry.boneWeights.push_back(binfos[i].weights[j]);
        }
    }
}

void Shape::processNode(const aiNode *node, const aiScene *scene, AlgineMTL *amtl) {
    // обработать все полигональные сетки в узле(если есть)
    for (size_t i = 0; i < node->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(Mesh::processMesh(mesh, scene, geometry, amtl));
        if (bonesPerVertex != 0) loadBones(mesh);
    }
    
    // выполнить ту же обработку и для каждого потомка узла
    for(size_t i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene, amtl);
    }
}

// _fullpath - path with textures, not necessary to ends with '/'
void Shape::loadTextures(const std::string &_fullpath) {
    // for loadTex we need path ends with '/'. If _fullpath empty, fullpath will be empty too
    std::string fullpath = _fullpath;
    if ((_fullpath.find_last_of('/') != (_fullpath.size() - 1)) && !fullpath.empty()) fullpath += '/';

    for (size_t i = 0; i < meshes.size(); i++) {
        #define mat meshes[i].mat
        if (!mat.texAmbientPath.empty()) mat.ambientTexture = loadTex(fullpath, mat.texAmbientPath);
        if (!mat.texDiffusePath.empty()) mat.diffuseTexture = loadTex(fullpath, mat.texDiffusePath);
        if (!mat.texSpecularPath.empty()) mat.specularTexture = loadTex(fullpath, mat.texSpecularPath);
        if (!mat.texNormalPath.empty()) mat.normalTexture = loadTex(fullpath, mat.texNormalPath);
        if (!mat.texReflectionPath.empty()) mat.reflectionTexture = loadTex(fullpath, mat.texReflectionPath);
        if (!mat.texJitterPath.empty()) mat.jitterTexture = loadTex(fullpath, mat.texJitterPath);
        #undef mat
    }
}

// load texture with full path = current path
void Shape::loadTextures() {
    loadTextures(Path::getWorkingDirectory());
}

void Shape::genBuffers() {
    #define genBufferForArray(buffer, array) if (!array.empty()) { glGenBuffers(1, &buffer); }
    
    genBufferForArray(buffers.vertices, geometry.vertices); // vertices
    genBufferForArray(buffers.normals, geometry.normals); // normals
    genBufferForArray(buffers.texCoords, geometry.texCoords); // texCoords
    genBufferForArray(buffers.tangents, geometry.tangents); // tangents
    genBufferForArray(buffers.bitangents, geometry.bitangents); // bitangents
    genBufferForArray(buffers.boneWeights, geometry.boneWeights); // bone weights
    genBufferForArray(buffers.boneIds, geometry.boneIds); // bone ids

    #define mkArrayBuffer(buffer, array, type) \
        glBindBuffer(GL_ARRAY_BUFFER, buffer); \
        glBufferData(GL_ARRAY_BUFFER, sizeof(type) * array.size(), &array[0], GL_STATIC_DRAW);

    #define mkArrayBufferf(buffer, array) if (!array.empty()) { mkArrayBuffer(buffer, array, float) }
    #define mkArrayBufferui(buffer, array) if (!array.empty()) { mkArrayBuffer(buffer, array, uint) }

    mkArrayBufferf(buffers.vertices, geometry.vertices); // vertices
    mkArrayBufferf(buffers.normals, geometry.normals); // normals
    mkArrayBufferf(buffers.texCoords, geometry.texCoords); // texCoords
    mkArrayBufferf(buffers.tangents, geometry.tangents); // tangents
    mkArrayBufferf(buffers.bitangents, geometry.bitangents); // bitangents
    mkArrayBufferf(buffers.boneWeights, geometry.boneWeights); // bone weights
    mkArrayBufferui(buffers.boneIds, geometry.boneIds); // bone ids

    // indices
    glGenBuffers(1, &buffers.indices);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.indices);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * geometry.indices.size(), &geometry.indices[0], GL_STATIC_DRAW);

    // to default
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    #undef mkArrayBuffer
    #undef mkArrayBufferf
    #undef mkArrayBufferui
    #undef genBufferForArray
}

void Shape::delBuffers() {
    glDeleteVertexArrays(vaos.size(), &vaos[0]);
    glDeleteBuffers(8, &buffers.vertices); // deletes ALL buffers from structure `buffers`
}

void Shape::recycleMeshes() {
    for (auto & mesh : meshes)
        mesh.recycle();
}

void Shape::init(const std::string &path, const uint params) {
    // Create an instance of the Importer class
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, params);

    // If the import failed, report it
    if (!scene) {
        std::cout << importer.GetErrorString() << "\n";
        return;
    }

    globalInverseTransform = getMat4(scene->mRootNode->mTransformation);
    glm::inverse(globalInverseTransform);

    rootNode = Node(scene->mRootNode);
    animations.reserve(scene->mNumAnimations); // allocate space for animations
    for (size_t i = 0; i < scene->mNumAnimations; i++)
        animations.emplace_back(scene->mAnimations[i]);

    std::string amtlPath = path.substr(0, path.find_last_of('.')) + ".amtl";
    AlgineMTL amtl;
    if (amtl.load(amtlPath)) processNode(scene->mRootNode, scene, &amtl);
    else processNode(scene->mRootNode, scene);
}

// creates VAO and adds it into `vaos` array
// note: this function will limit max bones per vertex to 4
// if you need more, you will have to create VAO manually
void Shape::createVAO(
        int inPosition, int inTexCoord, int inNormal,
        int inTangent, int inBitangent,
        int inBoneWeights, int inBoneIds
    ) {
    vaos.push_back(0); // allocate memory
    glGenVertexArrays(1, &vaos[vaos.size() - 1]);
    glBindVertexArray(vaos[vaos.size() - 1]);

    #define _pointer(location, count, buffer) if (location != -1) { glEnableVertexAttribArray(location); pointer(location, count, buffer); }
    #define _pointerui(location, count, buffer) if (location != -1) { glEnableVertexAttribArray(location); pointerui(location, count, buffer); }
    
    _pointer(inPosition, 3, buffers.vertices);
    _pointer(inNormal, 3, buffers.normals);
    _pointer(inTangent, 3, buffers.tangents);
    _pointer(inBitangent, 3, buffers.bitangents);
    _pointer(inTexCoord, 2, buffers.texCoords);

    if (bonesPerVertex != 0) {
        _pointer(inBoneWeights, 4, buffers.boneWeights);
        _pointerui(inBoneIds, 4, buffers.boneIds);
    }

    #undef _pointer
    #undef _pointerui

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.indices);
    glBindVertexArray(0);
}

void Shape::inverseNormals() {
    for (size_t i = 0; i < geometry.normals.size(); i++)
        geometry.normals[i] *= -1;
}

void Shape::setNodeTransform(const std::string &nodeName, const glm::mat4 &transformation) {
    rootNode.getNode(nodeName)->transformation = transformation;
}

void Shape::recycle() {
    // delete all materials and meshes
    recycleMeshes();
    delBuffers();
}

std::map<std::string, uint> Shape::loadedTextures;

// `Model` is a container for `Shape`, that have own `Animator` and transformations
Model::Model(const uint rotatorType) : Object3D (rotatorType) {
    type = Object3DTypeModel;
}

} // namespace algine
