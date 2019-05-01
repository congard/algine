/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

#ifndef ALGINE_MODEL_CPP
#define ALGINE_MODEL_CPP

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>
#include <vector>

#include <assimp/Importer.hpp>  // C++ importer interface
#include <assimp/scene.h>       // Output data structure
#include <assimp/postprocess.h> // Post processing flags

#include <algine/types.h>
#include "texture.cpp"
#include "mesh.cpp"
#include "node.cpp"
#include "bone.cpp"
#include "animation.cpp"

namespace algine {
struct Shape {
    private:
    static GLuint loadTex(const std::string &fullpath, const std::string &path) {
        std::string absolutePath = io::isAbsolutePath(path) ? path : fullpath + path;
        
        if (loadedTextures.find(absolutePath) != loadedTextures.end()) return loadedTextures[absolutePath]; // texture already loaded
        else {
            GLuint id = loadTexture(absolutePath.c_str());
            loadedTextures[absolutePath] = id;
            return id;
        }
    }

    size getBoneIndex(const std::string &name) {
        for (usize i = 0; i < bones.size(); i++)
            if (bones[i].name == name)
                return i;

        return -1;
    }

    void loadBones(Mesh &mesh, const aiMesh *aimesh) {
        std::vector<BoneInfo> binfos; // bone infos
        binfos.reserve(aimesh->mNumVertices); // allocating space
        
        // filling array
        for (size_t i = 0; i < aimesh->mNumVertices; i++)
            binfos.push_back(BoneInfo(bonesPerVertex));

        // loading bone data
        aiBone *bone;
        aiVertexWeight *vertexWeight;
        size boneIndex;
        for (usize i = 0; i < aimesh->mNumBones; i++) {
            bone = aimesh->mBones[i];
            std::string boneName(bone->mName.data);

            if ((boneIndex = getBoneIndex(boneName)) == -1) {
                boneIndex = bones.size();
                bones.push_back(Bone(boneName, getMat4(bone->mOffsetMatrix)));
            }

            for (usize j = 0; j < bone->mNumWeights; j++) {
                vertexWeight = &(bone->mWeights[j]);
                binfos[vertexWeight->mVertexId].add(boneIndex, vertexWeight->mWeight);
            }
        }

        // converting to a suitable view
        for (size_t i = 0; i < binfos.size(); i++) {
            for (size_t j = 0; j < binfos[i].size; j++) {
                mesh.boneIds.push_back(binfos[i].ids[j]);
                mesh.boneWeights.push_back(binfos[i].weights[j]);
            }
        }
    }

    public:
    static std::map<std::string, GLuint> loadedTextures;
    GLuint bonesPerVertex = 0;
    std::vector<Mesh> meshes;
    std::vector<Bone> bones;
    std::vector<Animation> animations;
    Node rootNode;
    glm::mat4 globalInverseTransform;
    
    void processNode(const aiNode *node, const aiScene *scene, AlgineMTL *amtl = nullptr) {
        // обработать все полигональные сетки в узле(если есть)
        for (size_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(Mesh::processMesh(mesh, scene, amtl));
            if (bonesPerVertex != 0) loadBones(meshes[meshes.size() - 1], mesh);	
        }
        
        // выполнить ту же обработку и для каждого потомка узла
        for(size_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene, amtl);
        }
    }
    
    // _fullpath - path with textures, not necessary to ends with '/'
    void loadTextures(const std::string &_fullpath) {
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
    void loadTextures() {
        loadTextures(io::getCurrentDir());
    }

    void genBuffers() {
        for (size_t i = 0; i < meshes.size(); i++) meshes[i].genBuffers();
    }

    void delBuffers() {
        for (size_t i = 0; i < meshes.size(); i++) meshes[i].delBuffers();
    }

    void recycleMeshes() {
        for (size_t i = 0; i < meshes.size(); i++) meshes[i].recycle();
    }

    void init(const std::string &path, const GLuint params) {
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
        for (size_t i = 0; i < scene->mNumAnimations; i++) animations.push_back(scene->mAnimations[i]);

        std::string amtlPath = path.substr(0, path.find_last_of(".")) + ".amtl";
        AlgineMTL amtl;
        if (amtl.load(amtlPath)) processNode(scene->mRootNode, scene, &amtl);
        else processNode(scene->mRootNode, scene);
    }

    void inverseNormals() {
        for (size_t i = 0; i < meshes.size(); i++)
            for (size_t j = 0; j < meshes[i].normals.size(); j++)
                meshes[i].normals[j] *= -1;
    }

    void recycle() {
        // delete all materials and meshes
        recycleMeshes();
    }
};

std::map<std::string, GLuint> Shape::loadedTextures;

// `Model` is a container for `Shape`, that have own `Animator` and transformations
struct Model {
    Shape *shape = nullptr;
    Animator *animator = nullptr;
    glm::mat4 translation, rotation, scaling, transformation;

    inline void translate(const glm::vec3 &tvec) {
        translation = glm::translate(translation, tvec);
    }

    inline void scale(const glm::vec3 &svec) {
        scaling = glm::scale(scaling, svec);
    }

    inline void rotate(float radians, const glm::vec3 &rvec) {
        rotation = glm::rotate(rotation, radians, rvec);
    }

    // translation matrix to identity, then `translate(pos)`
    inline void setPos(const glm::vec3 &pos) {
        translation = glm::mat4();
        translate(pos);
    }

    // Applies transformation. Result is transformation matrix
    inline void transform() {
        transformation = translation * rotation * scaling;
    }
    
    /* --- constructors, operators, destructor --- */

    Model(const Model &src) {
        shape = src.shape;
        animator = src.animator;
        translation = src.translation;
        rotation = src.rotation;
        scaling = src.scaling;
        transformation = src.transformation;
        
        #ifdef ALGINE_LOGGING
        std::cout << "Model copy constructor\n";
        #endif
    }

    Model(Model &&src) {
        src.swap(*this);
        #ifdef ALGINE_LOGGING
        std::cout << "Model move constructor\n";
        #endif
    }

    Model& operator = (const Model &rhs) {
        if (&rhs != this) Model(rhs).swap(*this);
        #ifdef ALGINE_LOGGING
        std::cout << "Model copy operator =\n";
        #endif
        return *this;
    }

    Model& operator = (Model &&rhs) {
        rhs.swap(*this);
        #ifdef ALGINE_LOGGING
        std::cout << "Model move operator =\n";
        #endif
        return *this;
    }

    Model(Shape *shape) {
        this->shape = shape;
    }

    Model() { /* empty */ }

    ~Model() {
        #ifdef ALGINE_LOGGING
        std::cout << "~Model()\n";
        #endif
    }

    /* --- --- */

    void swap(Model &other) {
        std::swap(shape, other.shape);
        std::swap(animator, other.animator);
        std::swap(translation, other.translation);
        std::swap(rotation, other.rotation);
        std::swap(scaling, other.scaling);
        std::swap(transformation, other.transformation);
    }
};

} // namespace algine

#endif
