#ifndef ALGINE_SHAPEMANAGER_H
#define ALGINE_SHAPEMANAGER_H

#include <algine/std/model/ShapePtr.h>
#include <algine/std/model/Shape.h>
#include <algine/std/AMTLManager.h>

#include <algine/core/ManagerBase.h>

class aiMesh;
class aiScene;

namespace algine {
class ShapeManager: public ManagerBase {
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

    enum class AMTLDumpMode {
        None,
        Path,
        Dump
    };

public:
    ShapeManager();

    void addParam(Param param);
    void addParams(const std::vector<Param> &params);
    void setParams(const std::vector<Param> &params);
    void addInputLayoutLocations(const InputLayoutShapeLocations &locations);
    void setInputLayoutLocations(const std::vector<InputLayoutShapeLocations> &locations);
    void setModelPath(const std::string &path);
    void setAMTLPath(const std::string &path);
    void setAMTL(const AMTLManager &amtlManager);
    void setBonesPerVertex(uint bonesPerVertex);

    const std::vector<Param>& getParams() const;
    const std::vector<InputLayoutShapeLocations>& getInputLayoutLocations() const;
    const std::string& getModelPath() const;
    const std::string& getAMTLPath() const;
    const AMTLManager& getAMTL() const;
    uint getBonesPerVertex() const;

    ShapePtr get();
    ShapePtr create();

    void setAMTLDumpMode(AMTLDumpMode mode);
    AMTLDumpMode getAMTLDumpMode() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    void load();
    void loadBones(const aiMesh *aimesh);
    void processNode(const aiNode *node, const aiScene *scene);
    void processMesh(const aiMesh *aimesh, const aiScene *scene);
    void genBuffers();

private:
    ShapePtr m_shape;
    AMTLDumpMode m_amtlDumpMode;

private:
    std::vector<float> m_vertices, m_normals, m_texCoords, m_tangents, m_bitangents, m_boneWeights;
    std::vector<uint> m_indices, m_boneIds;

private:
    std::vector<Param> m_params;
    std::vector<InputLayoutShapeLocations> m_locations;
    std::string m_modelPath, m_amtlPath;

    AMTLManager m_amtlManager;
    uint m_bonesPerVertex;
};
}

#endif //ALGINE_SHAPEMANAGER_H
