#ifndef ALGINE_SHAPECREATOR_H
#define ALGINE_SHAPECREATOR_H

#include <algine/std/model/InputLayoutShapeLocationsCreator.h>
#include <algine/std/model/ShapePtr.h>
#include <algine/std/model/Shape.h>
#include <algine/std/AMTLManager.h>

#include <algine/core/Creator.h>

struct aiMesh;
struct aiScene;

namespace algine {
class ShapeCreator: public Creator {
public:
    enum class Param {
        Triangulate,
        SortByPolygonType,
        CalcTangentSpace,
        JoinIdenticalVertices,
        InverseNormals,
        DisableBones
    };

    enum class AMTLDumpMode {
        None,
        Path,
        Dump
    };

public:
    ShapeCreator();

    void addParam(Param param);
    void addParams(const std::vector<Param> &params);
    void setParams(const std::vector<Param> &params);
    void addInputLayoutLocations(const InputLayoutShapeLocationsCreator &locations);
    void setInputLayoutLocations(const std::vector<InputLayoutShapeLocationsCreator> &locations);
    void addInputLayoutLocationsPath(const std::string &path);
    void setInputLayoutLocationsPaths(const std::vector<std::string> &paths);
    void setModelPath(const std::string &path);
    void setAMTLPath(const std::string &path);
    void setAMTL(const AMTLManager &amtlManager);
    void setBonesPerVertex(uint bonesPerVertex);
    void setClassName(const std::string &name);

    const std::vector<Param>& getParams() const;
    const std::vector<InputLayoutShapeLocationsCreator>& getInputLayoutLocations() const;
    const std::vector<std::string>& getInputLayoutLocationsPaths() const;
    const std::string& getModelPath() const;
    const std::string& getAMTLPath() const;
    const AMTLManager& getAMTL() const;
    uint getBonesPerVertex() const;
    const std::string& getClassName() const;

    const std::vector<float>& getVertices() const;
    void setVertices(const std::vector<float> &vertices);

    const std::vector<float>& getNormals() const;
    void setNormals(const std::vector<float> &normals);

    const std::vector<float>& getTexCoords() const;
    void setTexCoords(const std::vector<float> &texCoords);

    const std::vector<float>& getTangents() const;
    void setTangents(const std::vector<float> &tangents);

    const std::vector<float>& getBitangents() const;
    void setBitangents(const std::vector<float> &bitangents);

    const std::vector<float>& getBoneWeights() const;
    void setBoneWeights(const std::vector<float> &boneWeights);

    const std::vector<uint>& getBoneIds() const;
    void setBoneIds(const std::vector<uint> &boneIds);

    const std::vector<uint>& getIndices() const;
    void setIndices(const std::vector<uint> &indices);

    void loadFile();
    void loadShape();

    ShapePtr& getCurrentShape();

    ShapePtr get();
    ShapePtr create();

    void setAMTLDumpMode(AMTLDumpMode mode);
    AMTLDumpMode getAMTLDumpMode() const;

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
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
    std::vector<InputLayoutShapeLocationsCreator> m_locations;
    std::vector<std::string> m_locationsPaths;
    std::string m_modelPath, m_amtlPath;

    AMTLManager m_amtlManager;
    uint m_bonesPerVertex;

private:
    std::string m_className;
};
}

#endif //ALGINE_SHAPECREATOR_H
