#ifndef ALGINE_SHAPEMANAGER_H
#define ALGINE_SHAPEMANAGER_H

#include <algine/std/model/Shape.h>
#include <algine/std/AMTLManager.h>
#include <algine/types.h>

class aiMesh;
class aiScene;

namespace algine {
class ShapeManager {
public:
    enum class Param {
        Triangulate,
        SortByPolygonType,
        CalcTangentSpace,
        JoinIdenticalVertices,
        InverseNormals,
        PrepareAllAnimations,
        DisableBones
    };

public:
    ShapeManager();

    void load();
    void addParam(Param param);
    void addParams(const std::vector<Param> &params);
    void setParams(const std::vector<Param> &params);
    void setModelPath(const std::string &path);
    void setAMTLPath(const std::string &path);
    void setBonesPerVertex(uint bonesPerVertex);

    const std::vector<Param>& getParams() const;
    const std::string& getModelPath() const;
    const std::string& getAMTLPath() const;
    uint getBonesPerVertex() const;

    Shape *getShape() const;

private:
    void loadBones(const aiMesh *aimesh);
    void processNode(const aiNode *node, const aiScene *scene);
    void processMesh(const aiMesh *aimesh, const aiScene *scene);
    void genBuffers();

private:
    Shape *m_shape = nullptr;

private:
    std::vector<float> vertices, normals, texCoords, tangents, bitangents, boneWeights;
    std::vector<uint> indices, boneIds;

private:
    std::vector<Param> m_params;
    std::string m_modelPath, m_amtlPath;

    AMTLManager m_amtlManager;
    uint m_bonesPerVertex = 4;
};
}

#endif //ALGINE_SHAPEMANAGER_H
