/**
 * Algine: C++ OpenGL Engine
 * 
 * My telegram: https://t.me/congard
 * My github: https://github.com/congard
 * @author congard
 */

#ifndef ALGINE_MODEL_H
#define ALGINE_MODEL_H

#include <algine/types.h>
#include <algine/material.h>
#include <algine/bone.h>
#include <algine/animation.h>
#include <algine/object3d.h>
#include <vector>
#include <map>
#include <assimp/scene.h> // Output data structure

namespace algine {
struct Geometry {
    std::vector<float> vertices, normals, texCoords, tangents, bitangents, boneWeights;
    std::vector<uint> indices, boneIds;
};

class Mesh {
public:
    uint start = 0, count = 0;
    Material mat;

    void delMaterial();
    void recycle();
    static Mesh processMesh(const aiMesh *aimesh, const aiScene *scene, Geometry &geometry, AlgineMTL *amtl = nullptr);
};

class Shape {
protected:
    static uint loadTex(const std::string &fullpath, const std::string &path);
    size getBoneIndex(const std::string &name);
    void loadBones(const aiMesh *aimesh);

public:
    static std::map<std::string, uint> loadedTextures;
    uint bonesPerVertex = 0;
    std::vector<Mesh> meshes;
    std::vector<Bone> bones;
    std::vector<Animation> animations;
    Node rootNode;
    glm::mat4 globalInverseTransform;

    std::vector<uint> vaos;
    Geometry geometry;

    struct Buffers {
        uint vertices, normals, texCoords, tangents, bitangents, indices, boneWeights, boneIds;
    } buffers;

    void processNode(const aiNode *node, const aiScene *scene, AlgineMTL *amtl = nullptr);
    void loadTextures(const std::string &_fullpath); // _fullpath - path with textures, not necessary to ends with '/'
    void loadTextures(); // load texture with full path = current path
    void genBuffers();
    void delBuffers();
    void recycleMeshes();
    void init(const std::string &path, const uint params);

    // creates VAO and adds it into `vaos` array
    // note: this function will limit max bones per vertex to 4
    // if you need more, you will have to create VAO manually
    void createVAO(
            int inPosition, int inTexCoord = -1, int inNormal = -1,
            int inTangent = -1, int inBitangent = -1,
            int inBoneWeights = -1, int inBoneIds = -1
        );

    void inverseNormals();
    void setNodeTransform(const std::string &nodeName, const glm::mat4 &transformation);
    void recycle();
};

// `Model` is a container for `Shape`, that have own `Animator` and transformations
class Model: public Object3D {
public:
    Shape *shape = nullptr;
    Animator *animator = nullptr;

    Model(const uint rotatorType = Rotator::RotatorTypeSimple);
};

} // namespace algine

#endif // ALGINE_MODEL_H
