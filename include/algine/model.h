#ifndef ALGINE_MODEL_H
#define ALGINE_MODEL_H

#include <algine/types.h>
#include <algine/material.h>
#include <algine/AMTLLoader.h>
#include <algine/bone.h>
#include <algine/animation.h>
#include <algine/object3d.h>
#include <algine/ArrayBuffer.h>
#include <algine/IndexBuffer.h>
#include <algine/InputLayout.h>
#include <vector>
#include <map>
#include <assimp/scene.h> // Output data structure

namespace algine {
struct Geometry {
    std::vector<float> vertices, normals, texCoords, tangents, bitangents, boneWeights;
    std::vector<uint> indices, boneIds;
};

struct Mesh {
    uint start = 0, count = 0;
    Material material;
};

class Shape {
public:
    void delBuffers();

    // TODO: move it to ShapeLoader
    // creates InputLayout and adds it into inputLayouts array
    // note: this function will limit max bones per vertex to 4
    // if you need more, you will have to create InputLayout manually
    void createInputLayout(
            int inPosition, int inTexCoord = -1, int inNormal = -1,
            int inTangent = -1, int inBitangent = -1,
            int inBoneWeights = -1, int inBoneIds = -1
        );

    void setNodeTransform(const std::string &nodeName, const glm::mat4 &transformation);
    void recycle();

public:
    std::vector<Mesh> meshes;
    std::vector<Bone> bones;
    std::vector<Animation> animations;
    glm::mat4 globalInverseTransform;
    std::vector<InputLayout*> inputLayouts;
    Node rootNode;
    Geometry geometry;
    uint bonesPerVertex = 0;

    struct Buffers {
        ArrayBuffer *vertices, *normals, *texCoords, *tangents, *bitangents, *boneWeights, *boneIds;
        IndexBuffer *indices;
    } buffers;
};

// `Model` is a container for `Shape`, that have own `Animator` and transformations
class Model: public rotatable, public translatable, public scalable {
public:
    explicit Model(uint rotatorType = Rotator::RotatorTypeSimple);

    void updateMatrix();

public:
    Shape *shape = nullptr;
    Animator *animator = nullptr;
    glm::mat4 m_transform;
};

class ShapeLoader {
protected:
    void loadBones(const aiMesh *aimesh);
    void processNode(const aiNode *node, const aiScene *scene);
    void processMesh(const aiMesh *aimesh, const aiScene *scene);
    void loadTextures();
    void genBuffers();

protected:
    struct MaterialTexPaths {
        std::string ambient, diffuse, specular, normal, reflection, jitter;
    };

    std::vector<MaterialTexPaths> m_materialTexPaths;
    AMTLLoader *m_amtlLoader = nullptr; // NOTE: exists only during load()!

    struct LoadedTexture {
        std::string path;
        std::shared_ptr<Texture2D> texture;
        std::map<uint, uint> params;

        LoadedTexture(const std::string &path, const std::shared_ptr<Texture2D> &texture,
                const std::map<uint, uint> &params);
    };

    std::vector<LoadedTexture> m_modelLoadedTextures;
    static std::vector<LoadedTexture> m_globalLoadedTextures;
    static int getLoadedTextureIndex(const std::vector<LoadedTexture> *loadedTextures, const std::string &path,
            const std::map<uint, uint> &params);

public:
    enum Params {
        Triangulate,
        SortByPolygonType,
        CalcTangentSpace,
        JoinIdenticalVertices,
        InverseNormals
    };

    ShapeLoader();

    void load();

    void addParam(uint param);
    void setModelPath(const std::string &path);
    void setTexturesPath(const std::string &path);
    void setDefaultTexturesParams(const std::map<uint, uint> &params);

    template<typename...Args>
    void addParams(Args...args) {
        int params[] = {args...};
        for (uint i = 0; i < sizeof...(args); i++)
            addParam(params[i]);
    }

    Shape *getShape() const;

public:
    Shape *m_shape = nullptr;
    std::vector<uint> m_params;
    std::string m_modelPath, m_texturesPath;

    std::map<uint, uint> m_defaultTexturesParams = std::map<uint, uint> {
            std::pair<uint, uint> {Texture::WrapU, Texture::Repeat},
            std::pair<uint, uint> {Texture::WrapV, Texture::Repeat},
            std::pair<uint, uint> {Texture::MinFilter, Texture::Linear},
            std::pair<uint, uint> {Texture::MagFilter, Texture::Linear}
    };
};

} // namespace algine

#endif // ALGINE_MODEL_H
