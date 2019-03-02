/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My gitlab: https://gitlab.com/congard
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

#include "texture.cpp"
#include "mesh.cpp"

namespace algine {
struct ModelBuffers {
    private:
    GLuint loadTex(const std::string &fullpath, const std::string &path) {
        std::string absolutePath = io::isAbsolutePath(path) ? path : fullpath + path;
        
        if (loadedTextures.find(absolutePath) != loadedTextures.end()) return loadedTextures[absolutePath]; // texture already loaded
        else {
            GLuint id = loadTexture(absolutePath.c_str());
            loadedTextures[absolutePath] = id;
            return id;
        }
    }

    public:
    static std::map<std::string, GLuint> loadedTextures;
    std::vector<Mesh> meshes;
    
    void processNode(const aiNode *node, const aiScene *scene, AlgineMTL *amtl = nullptr) {
        // обработать все полигональные сетки в узле(если есть)
        for (size_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[node->mMeshes[i]]; 
            meshes.push_back(Mesh::processMesh(mesh, scene, amtl));			
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
        // DELETE ALL MATERIALS AND MESHES
        recycleMeshes();
    }
};

std::map<std::string, GLuint> ModelBuffers::loadedTextures;

class Model {
    public:
        ModelBuffers *buffers;
        glm::vec3 origin, angles;
        
        /* --- constructors, operators, destructor --- */

        Model(const Model &src) {
            buffers = src.buffers;
            angles = src.angles;
            origin = src.origin;
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

        Model(ModelBuffers *buffers) {
            this->buffers = buffers;
        }

        Model() { /* for arrays constructor */ }

        ~Model() {
            #ifdef ALGINE_LOGGING
            std::cout << "~Model()\n";
            #endif
        }

        /* --- --- */

        void swap(Model &other) {
            std::swap(buffers, other.buffers);
            std::swap(origin, other.origin);
            std::swap(angles, other.angles);
        }
};

} // namespace algine

#endif
