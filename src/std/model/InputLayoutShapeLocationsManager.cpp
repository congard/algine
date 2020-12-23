#include <algine/std/model/InputLayoutShapeLocationsManager.h>

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

InputLayoutShapeLocationsManager::Location::Location(string locationName)
    : m_name(move(locationName)),
      m_location(InputLayoutShapeLocations::None)
{
    // see initializer list above
}

InputLayoutShapeLocationsManager::Location::Location(const char *locationName)
    : m_name(locationName),
      m_location(InputLayoutShapeLocations::None)
{
    // see initializer list above
}

InputLayoutShapeLocationsManager::Location::Location(int location)
    : m_location(location)
{
    // see initializer list above
}

InputLayoutShapeLocationsManager::Location::Location()
    : m_location(InputLayoutShapeLocations::None)
{
    // see initializer list above
}

const string& InputLayoutShapeLocationsManager::Location::getLocationName() const {
    return m_name;
}

int InputLayoutShapeLocationsManager::Location::getLocation() const {
    return m_location;
}

bool InputLayoutShapeLocationsManager::Location::hasLocationName() const {
    return !m_name.empty();
}

bool InputLayoutShapeLocationsManager::Location::hasLocation() const {
    return m_location != InputLayoutShapeLocations::None;
}

InputLayoutShapeLocationsManager::InputLayoutShapeLocationsManager(const InputLayoutShapeLocations &locations)
    : InputLayoutShapeLocationsManager()
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

InputLayoutShapeLocationsManager::InputLayoutShapeLocationsManager()
    : m_shaderDumpMode(ShaderProgramDumpMode::None)
{
    // see initializer list above
}

void InputLayoutShapeLocationsManager::addLocation(Location::Type type, const Location &location) {
    if (location.hasLocation() || location.hasLocationName()) {
        m_locations[type] = location;
    }
}

void InputLayoutShapeLocationsManager::setLocations(const Locations &locations) {
    m_locations = locations;
}

void InputLayoutShapeLocationsManager::setShaderProgram(const ShaderProgramManager &manager) {
    setShaderProgramDumpMode(ShaderProgramDumpMode::Dump);

    m_shader = manager;
}

void InputLayoutShapeLocationsManager::setShaderProgramName(const string &name) {
    setShaderProgramDumpMode(ShaderProgramDumpMode::Name);

    m_shaderName = name;
}

void InputLayoutShapeLocationsManager::setShaderProgramPath(const string &path) {
    setShaderProgramDumpMode(ShaderProgramDumpMode::Path);

    m_shaderPath = path;
}

void InputLayoutShapeLocationsManager::setShaderProgramDumpMode(ShaderProgramDumpMode mode) {
    m_shaderDumpMode = mode;
}

void InputLayoutShapeLocationsManager::setShaderProgramPtr(const ShaderProgramPtr &program) {
    m_programPtr = program;
}

const InputLayoutShapeLocationsManager::Locations& InputLayoutShapeLocationsManager::getLocations() const {
    return m_locations;
}

const ShaderProgramManager& InputLayoutShapeLocationsManager::getShaderProgram() const {
    return m_shader;
}

const string& InputLayoutShapeLocationsManager::getShaderProgramName() const {
    return m_shaderName;
}

const string& InputLayoutShapeLocationsManager::getShaderProgramPath() const {
    return m_shaderPath;
}

InputLayoutShapeLocationsManager::ShaderProgramDumpMode
InputLayoutShapeLocationsManager::getShaderProgramDumpMode() const {
    return m_shaderDumpMode;
}

const ShaderProgramPtr& InputLayoutShapeLocationsManager::getShaderProgramPtr() const {
    return m_programPtr;
}

InputLayoutShapeLocations InputLayoutShapeLocationsManager::create() {
    switch (m_shaderDumpMode) {
        case ShaderProgramDumpMode::Name: {
            m_programPtr = ShaderProgram::getByName(m_shaderName);
            break;
        }
        case ShaderProgramDumpMode::Path: {
            ShaderProgramManager manager;
            manager.setWorkingDirectory(m_workingDirectory);
            manager.importFromFile(m_shaderPath);

            m_programPtr = manager.get();

            break;
        }
        case ShaderProgramDumpMode::Dump: {
            m_programPtr = m_shader.get();
            break;
        }
        default: {
            // Explicitly left empty
        }
    }

    InputLayoutShapeLocations locations;

    auto loadLocationImpl = [&](int &location, Location::Type type)
    {
        if (auto it = m_locations.find(type); it != m_locations.end()) {
            auto &data = it->second;

            if (const auto &name = data.getLocationName(); !name.empty()) {
                if (m_programPtr != nullptr) {
                    m_programPtr->loadUniformLocation(name);
                    location = m_programPtr->getLocation(name);
                } else {
                    throw runtime_error("Can't load location from null ShaderProgram");
                }
            } else {
                location = data.getLocation();
            }
        }
    };

#define loadLocation(name, type) loadLocationImpl(locations.name, Location::Type::type)

    loadLocation(position, Position);
    loadLocation(texCoord, TexCoord);
    loadLocation(normal, Normal);
    loadLocation(tangent, Tangent);
    loadLocation(bitangent, Bitangent);
    loadLocation(boneWeights, BoneWeights);
    loadLocation(boneIds, BoneIds);

#undef loadLocation

    return locations;
}

#define type_str(name) if (type == InputLayoutShapeLocationsManager::Location::Type::name) return Config::name

inline auto getLocationKey(InputLayoutShapeLocationsManager::Location::Type type) {
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

void InputLayoutShapeLocationsManager::import(const JsonHelper &jsonHelper) {
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
            ShaderProgramManager manager;
            manager.setWorkingDirectory(m_workingDirectory);
            manager.import(program[Dump]);

            m_shader = manager;
            m_shaderDumpMode = ShaderProgramDumpMode::Dump;
        }
    }
}

JsonHelper InputLayoutShapeLocationsManager::dump() {
    using namespace Config;

    json config;

    auto write = [&](const string &key, const auto &location)
    {
        using T = remove_const_t<remove_reference_t<typeof(location)>>;

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
