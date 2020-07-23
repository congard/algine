#ifndef ALGINE_SHAPELOADER_H
#define ALGINE_SHAPELOADER_H

#include <algine/types.h>
#include <algine/std/model/Shape.h>

class aiMesh;
class aiScene;

namespace algine {
class AMTLLoader;

class ShapeLoader {
public:
    enum Params {
        Triangulate,
        SortByPolygonType,
        CalcTangentSpace,
        JoinIdenticalVertices,
        InverseNormals,
        PrepareAllAnimations,
        DisableBones
    };

    ShapeLoader();

    void load();
    void addParam(uint param);
    void setModelPath(const std::string &path);
    void setTexturesPath(const std::string &path);
    void setDefaultTexturesParams(const std::map<uint, uint> &params);
    void setBonesPerVertex(uint bonesPerVertex);

    template<typename...Args>
    void addParams(Args...args) {
        int params[] = {args...};
        for (uint i = 0; i < sizeof...(args); i++)
            addParam(params[i]);
    }

    Shape *getShape() const;

public:
    Shape *m_shape = nullptr;

private:
    struct MaterialTexPaths {
        std::string ambient, diffuse, specular, normal, reflection, jitter;
    };

    struct LoadedTexture {
        std::string path;
        std::shared_ptr<Texture2D> texture;
        std::map<uint, uint> params;

        LoadedTexture(const std::string &path, const std::shared_ptr<Texture2D> &texture,
                      const std::map<uint, uint> &params);
    };

    struct Geometry {
        std::vector<float> vertices, normals, texCoords, tangents, bitangents, boneWeights;
        std::vector<uint> indices, boneIds;
    } geometry;

private:
    void loadBones(const aiMesh *aimesh);
    void processNode(const aiNode *node, const aiScene *scene);
    void processMesh(const aiMesh *aimesh, const aiScene *scene);
    void loadTextures();
    void genBuffers();
    static int getLoadedTextureIndex(const std::vector<LoadedTexture> *loadedTextures, const std::string &path,
                                     const std::map<uint, uint> &params);

private:
    std::vector<MaterialTexPaths> m_materialTexPaths;
    std::vector<uint> m_params;
    std::string m_modelPath, m_texturesPath;

    std::map<uint, uint> m_defaultTexturesParams = std::map<uint, uint> {
            std::pair<uint, uint> {Texture::WrapU, Texture::Repeat},
            std::pair<uint, uint> {Texture::WrapV, Texture::Repeat},
            std::pair<uint, uint> {Texture::MinFilter, Texture::Linear},
            std::pair<uint, uint> {Texture::MagFilter, Texture::Linear}
    };

    AMTLLoader *m_amtlLoader = nullptr; // NOTE: exists only during load()!
    uint m_bonesPerVertex = 4;

    std::vector<LoadedTexture> m_modelLoadedTextures;
    static std::vector<LoadedTexture> m_globalLoadedTextures;
};
}

#endif //ALGINE_SHAPELOADER_H
