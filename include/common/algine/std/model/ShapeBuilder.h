#ifndef ALGINE_SHAPEBUILDER_H
#define ALGINE_SHAPEBUILDER_H

#include <algine/std/model/Shape.h>
#include <algine/std/AMTLManager.h>

#include <algine/core/Builder.h>

struct aiMesh;
struct aiScene;

namespace algine {
class ShapeBuilder: public Builder {
    AL_BUILDER(Shape)

public:
    enum class Param {
        Triangulate,
        SortByPolygonType,
        CalcTangentSpace,
        JoinIdenticalVertices,
        InverseNormals,
        DisableBones
    };

    template<typename T>
    struct BufferData {
        inline auto operator->() { return &data; }

        std::vector<T> data;
        int usage = Buffer::StaticDraw;
    };

public:
    ShapeBuilder();

    void addParam(Param param);
    void addParams(const std::vector<Param> &params);
    void setParams(const std::vector<Param> &params);
    void addInputLayoutLocations(const InputLayoutShapeLocations &locations);
    void setInputLayoutLocations(const std::vector<InputLayoutShapeLocations> &locations);
    void setModelPath(const std::string &path);
    void setAMTL(const AMTLManager &amtlManager);
    void setBonesPerVertex(uint bonesPerVertex);
    void setClassName(const std::string &name);

    const std::vector<Param>& getParams() const;
    const std::vector<InputLayoutShapeLocations>& getInputLayoutLocations() const;
    const std::string& getModelPath() const;
    const AMTLManager& getAMTL() const;
    uint getBonesPerVertex() const;
    const std::string& getClassName() const;

    const BufferData<float>& getVertices() const;
    void setVertices(const BufferData<float> &vertices);

    const BufferData<float>& getNormals() const;
    void setNormals(const BufferData<float> &normals);

    const BufferData<float>& getTexCoords() const;
    void setTexCoords(const BufferData<float> &texCoords);

    const BufferData<float>& getTangents() const;
    void setTangents(const BufferData<float> &tangents);

    const BufferData<float>& getBitangents() const;
    void setBitangents(const BufferData<float> &bitangents);

    const BufferData<float>& getBoneWeights() const;
    void setBoneWeights(const BufferData<float> &boneWeights);

    const BufferData<uint>& getBoneIds() const;
    void setBoneIds(const BufferData<uint> &boneIds);

    const BufferData<uint>& getIndices() const;
    void setIndices(const BufferData<uint> &indices);

    void loadFile();
    void loadShape();
    void applyParams();
    void genBuffers();
    void createInputLayouts();

    Shape* getCurrentShape();

protected:
    Object* createImpl() override;
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

private:
    void loadBones(const aiMesh *aimesh);
    void processNode(const aiNode *node, const aiScene *scene);
    void processMesh(const aiMesh *aimesh, const aiScene *scene);

private:
    Shape *m_shape;

private:
    BufferData<float> m_vertices, m_normals, m_texCoords, m_tangents, m_bitangents, m_boneWeights;
    BufferData<uint> m_indices, m_boneIds;

private:
    std::vector<Param> m_params;
    std::vector<InputLayoutShapeLocations> m_locations;
    std::string m_modelPath;

    AMTLManager m_amtlManager;
    uint m_bonesPerVertex;

private:
    std::string m_className;
};
}

#endif //ALGINE_SHAPEBUILDER_H
