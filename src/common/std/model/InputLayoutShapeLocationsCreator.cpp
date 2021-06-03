#include <algine/std/model/InputLayoutShapeLocationsCreator.h>

#include <algine/core/JsonHelper.h>

#include <utility>

#include "internal/ConfigStrings.h"

using namespace nlohmann;
using namespace std;

namespace algine {
namespace Config {
constant(Position, "position");
constant(TexCoord, "texCoord");
constant(Normal, "normal");
constant(Tangent, "tangent");
constant(Bitangent, "bitangent");
constant(BoneWeights, "boneWeights");
constant(BoneIds, "boneIds");

constant(Locations, "locations");
constant(ShaderProgram, "shaderProgram");
}

namespace Default {
constexpr int Location = -1;
}

InputLayoutShapeLocationsCreator::Location::Location(string locationName)
    : m_name(move(locationName)),
      m_location(InputLayoutShapeLocations::None) {}

InputLayoutShapeLocationsCreator::Location::Location(const char *locationName)
    : m_name(locationName),
      m_location(InputLayoutShapeLocations::None) {}

InputLayoutShapeLocationsCreator::Location::Location(int location)
    : m_location(location) {}

InputLayoutShapeLocationsCreator::Location::Location()
    : m_location(InputLayoutShapeLocations::None) {}

const string& InputLayoutShapeLocationsCreator::Location::getLocationName() const {
    return m_name;
}

int InputLayoutShapeLocationsCreator::Location::getLocation() const {
    return m_location;
}

bool InputLayoutShapeLocationsCreator::Location::hasLocationName() const {
    return !m_name.empty();
}

bool InputLayoutShapeLocationsCreator::Location::hasLocation() const {
    return m_location != InputLayoutShapeLocations::None;
}

InputLayoutShapeLocationsCreator::InputLayoutShapeLocationsCreator(const InputLayoutShapeLocations &locations)
    : InputLayoutShapeLocationsCreator()
{
#define add(type, name) addLocation(Location::Type::type, locations.name)

    add(Position, position);
    add(TexCoord, texCoord);
    add(Normal, normal);
    add(Tangent, tangent);
    add(Bitangent, bitangent);
    add(BoneWeights, boneWeights);
    add(BoneIds, boneIds);

#undef add
}

InputLayoutShapeLocationsCreator::InputLayoutShapeLocationsCreator()
    : m_shaderDumpMode(ShaderProgramDumpMode::None) {}

void InputLayoutShapeLocationsCreator::addLocation(Location::Type type, const Location &location) {
    if (location.hasLocation() || location.hasLocationName()) {
        m_locations[type] = location;
    }
}

void InputLayoutShapeLocationsCreator::setLocations(const Locations &locations) {
    m_locations = locations;
}

void InputLayoutShapeLocationsCreator::setShaderProgram(const ShaderProgramCreator &creator) {
    setShaderProgramDumpMode(ShaderProgramDumpMode::Dump);

    m_shader = creator;
}

void InputLayoutShapeLocationsCreator::setShaderProgramName(const string &name) {
    setShaderProgramDumpMode(ShaderProgramDumpMode::Name);

    m_shaderName = name;
}

void InputLayoutShapeLocationsCreator::setShaderProgramPath(const string &path) {
    setShaderProgramDumpMode(ShaderProgramDumpMode::Path);

    m_shaderPath = path;
}

void InputLayoutShapeLocationsCreator::setShaderProgramDumpMode(ShaderProgramDumpMode mode) {
    m_shaderDumpMode = mode;
}

void InputLayoutShapeLocationsCreator::setShaderProgramPtr(const ShaderProgramPtr &program) {
    m_programPtr = program;
}

const InputLayoutShapeLocationsCreator::Locations& InputLayoutShapeLocationsCreator::getLocations() const {
    return m_locations;
}

const ShaderProgramCreator& InputLayoutShapeLocationsCreator::getShaderProgram() const {
    return m_shader;
}

const string& InputLayoutShapeLocationsCreator::getShaderProgramName() const {
    return m_shaderName;
}

const string& InputLayoutShapeLocationsCreator::getShaderProgramPath() const {
    return m_shaderPath;
}

InputLayoutShapeLocationsCreator::ShaderProgramDumpMode
InputLayoutShapeLocationsCreator::getShaderProgramDumpMode() const {
    return m_shaderDumpMode;
}

const ShaderProgramPtr& InputLayoutShapeLocationsCreator::getShaderProgramPtr() const {
    return m_programPtr;
}

InputLayoutShapeLocations InputLayoutShapeLocationsCreator::create() {
    switch (m_shaderDumpMode) {
        case ShaderProgramDumpMode::Name: {
            m_programPtr = ShaderProgram::getByName(m_shaderName);
            break;
        }
        case ShaderProgramDumpMode::Path: {
            ShaderProgramCreator creator;
            creator.setIOSystem(io());
            creator.setWorkingDirectory(m_workingDirectory);
            creator.importFromFile(m_shaderPath);

            m_programPtr = creator.get();

            break;
        }
        case ShaderProgramDumpMode::Dump: {
            m_shader.setIOSystem(io());
            m_programPtr = m_shader.get();
            break;
        }
        default: {
            // Explicitly left empty
        }
    }

    InputLayoutShapeLocations locations;

    auto loadLocation = [&](int &location, Location::Type type)
    {
        if (auto it = m_locations.find(type); it != m_locations.end()) {
            auto &data = it->second;

            if (const auto &name = data.getLocationName(); !name.empty()) {
                if (m_programPtr != nullptr) {
                    m_programPtr->loadUniformLocation(name); // TODO: uniform?
                    location = m_programPtr->getLocation(name);
                } else {
                    throw runtime_error("Can't load location from null ShaderProgram");
                }
            } else {
                location = data.getLocation();
            }
        }
    };

    loadLocation(locations.position, Location::Type::Position);
    loadLocation(locations.texCoord, Location::Type::TexCoord);
    loadLocation(locations.normal, Location::Type::Normal);
    loadLocation(locations.tangent, Location::Type::Tangent);
    loadLocation(locations.bitangent, Location::Type::Bitangent);
    loadLocation(locations.boneWeights, Location::Type::BoneWeights);
    loadLocation(locations.boneIds, Location::Type::BoneIds);

    return locations;
}

#define type_str(name) if (type == InputLayoutShapeLocationsCreator::Location::Type::name) return Config::name

inline auto getLocationKey(InputLayoutShapeLocationsCreator::Location::Type type) {
    type_str(Position);
    type_str(TexCoord);
    type_str(Normal);
    type_str(Tangent);
    type_str(Bitangent);
    type_str(BoneWeights);
    type_str(BoneIds);

    throw runtime_error("Unknown location type " + to_string(static_cast<int>(type)));
}

#undef type_str

void InputLayoutShapeLocationsCreator::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    const json &config = jsonHelper.json;

    // read locations
    auto read = [&](Location::Type type)
    {
        const json &locations = config[Config::Locations];

        string key = getLocationKey(type);

        if (locations.contains(key)) {
            json value = locations[key];

            if (value.is_number_integer()) {
                m_locations[type] = static_cast<int>(value);
            } else {
                m_locations[type] = static_cast<string>(value);
            }
        }
    };

    read(Location::Type::Position);
    read(Location::Type::TexCoord);
    read(Location::Type::Normal);
    read(Location::Type::Tangent);
    read(Location::Type::Bitangent);
    read(Location::Type::BoneWeights);
    read(Location::Type::BoneIds);

    // read shader program
    if (config.contains(Config::ShaderProgram)) {
        const json &program = config[Config::ShaderProgram];

        if (program.contains(Name)) {
            m_shaderName = program[Name];
            m_shaderDumpMode = ShaderProgramDumpMode::Name;
        } else if (program.contains(Path)) {
            m_shaderPath = program[Path];
            m_shaderDumpMode = ShaderProgramDumpMode::Path;
        } else if (program.contains(Dump)) {
            ShaderProgramCreator creator;
            creator.setWorkingDirectory(m_workingDirectory);
            creator.import(program[Dump]);

            m_shader = creator;
            m_shaderDumpMode = ShaderProgramDumpMode::Dump;
        }
    }
}

JsonHelper InputLayoutShapeLocationsCreator::dump() {
    using namespace Config;

    json config;

    auto write = [&](const string &key, const auto &location)
    {
        using T = remove_const_t<remove_reference_t<decltype(location)>>;

        if constexpr (is_same_v<T, string>) {
            if (!location.empty()) {
                config[Config::Locations][key] = location;
            }
        }

        if constexpr (is_same_v<T, int>) {
            if (location != InputLayoutShapeLocations::None) {
                config[Config::Locations][key] = location;
            }
        }
    };

    // write locations
    for (const auto & location : m_locations) {
        auto key = getLocationKey(location.first);

        if (location.second.getLocation() != InputLayoutShapeLocations::None) {
            write(key, location.second.getLocation());
        } else {
            write(key, location.second.getLocationName());
        }
    }

    // write shader program
    // lambda just for code reduction in switch statement
    auto program = [&]() -> json&
    {
        return config[Config::ShaderProgram];
    };

    switch (m_shaderDumpMode) {
        case ShaderProgramDumpMode::Name: {
            program()[Name] = m_shaderName;
            break;
        }
        case ShaderProgramDumpMode::Path: {
            program()[Path] = m_shaderPath;
            break;
        }
        case ShaderProgramDumpMode::Dump: {
            program()[Dump] = m_shader.dump().get();
            break;
        }
        default: {
            // Explicitly left empty
        }
    }

    return config;
}
}
