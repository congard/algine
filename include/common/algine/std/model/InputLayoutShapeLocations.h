#ifndef ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
#define ALGINE_INPUTLAYOUTSHAPELOCATIONS_H

#include <algine/core/lua/Scriptable.h>
#include <algine/core/shader/ShaderProgramPtr.h>

#include <string>
#include <map>

namespace algine {
class InputLayoutShapeLocations: public Scriptable {
public:
    constexpr static int None = -1;
    constexpr static auto Position = "position";
    constexpr static auto TexCoord = "texCoord";
    constexpr static auto Normal = "normal";
    constexpr static auto Tangent = "tangent";
    constexpr static auto Bitangent = "bitangent";
    constexpr static auto BoneWeights = "boneWeights";
    constexpr static auto BoneIds = "boneIds";

public:
    InputLayoutShapeLocations() = default;
    InputLayoutShapeLocations(const ShaderProgramPtr &program, const std::map<std::string, std::string> &locations);

    int operator[](const std::string &name) const;
    int& operator[](const std::string &name);

    void load(const ShaderProgramPtr &program, const std::map<std::string, std::string> &locations);

    int get(const std::string &name) const;
    void set(const std::string &name, int value);

    static InputLayoutShapeLocations fromFile(const std::string &path);
    static InputLayoutShapeLocations fromScript(const std::string &script);

protected:
    void exec(const std::string &s, bool path, Lua *lua, sol::global_table *tenv) override;

public:
    std::map<std::string, int> data;
};
}

#endif //ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
