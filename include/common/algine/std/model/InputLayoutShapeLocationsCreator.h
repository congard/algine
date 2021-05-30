#ifndef ALGINE_INPUTLAYOUTSHAPELOCATIONSCREATOR_H
#define ALGINE_INPUTLAYOUTSHAPELOCATIONSCREATOR_H

#include <algine/core/shader/ShaderProgramCreator.h>
#include <algine/core/transfer/FileTransferable.h>

#include <algine/std/model/InputLayoutShapeLocations.h>

#include <map>

namespace algine {
class InputLayoutShapeLocationsCreator: public FileTransferable {
public:
    class Location {
    public:
        enum class Type {
            Position,
            TexCoord,
            Normal,
            Tangent,
            Bitangent,
            BoneWeights,
            BoneIds
        };

    public:
        // do not mark ctors as explicit

        Location(std::string locationName);
        Location(const char *locationName);
        Location(int location);
        Location();

        const std::string& getLocationName() const;
        int getLocation() const;

        bool hasLocationName() const;
        bool hasLocation() const;

    private:
        std::string m_name;
        int m_location;
    };

    using Locations = std::map<Location::Type, Location>;

public:
    enum class ShaderProgramDumpMode {
        None,
        Name,
        Path,
        Dump
    };

public:
    InputLayoutShapeLocationsCreator(const InputLayoutShapeLocations &locations);
    InputLayoutShapeLocationsCreator();

    void addLocation(Location::Type type, const Location &location);

    void setLocations(const Locations &locations);
    void setShaderProgram(const ShaderProgramCreator &creator);
    void setShaderProgramName(const std::string &name);
    void setShaderProgramPath(const std::string &path);
    void setShaderProgramDumpMode(ShaderProgramDumpMode mode);
    void setShaderProgramPtr(const ShaderProgramPtr &program);

    const Locations& getLocations() const;
    const ShaderProgramCreator& getShaderProgram() const;
    const std::string& getShaderProgramName() const;
    const std::string& getShaderProgramPath() const;
    ShaderProgramDumpMode getShaderProgramDumpMode() const;
    const ShaderProgramPtr& getShaderProgramPtr() const;

    InputLayoutShapeLocations create();

    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

private:
    Locations m_locations;
    ShaderProgramPtr m_programPtr;
    ShaderProgramDumpMode m_shaderDumpMode;
    ShaderProgramCreator m_shader;
    std::string m_shaderName, m_shaderPath;
};
}

#endif //ALGINE_INPUTLAYOUTSHAPELOCATIONSCREATOR_H
