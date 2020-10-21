#include <algine/std/model/InputLayoutShapeLocations.h>

#include <algine/core/JsonHelper.h>

using namespace std;
using namespace nlohmann;

#define constant(name, val) constexpr char name[] = val

namespace algine {
namespace Config {
constant(Position, "position");
constant(TexCoord, "texCoord");
constant(Normal, "normal");
constant(Tangent, "tangent");
constant(Bitangent, "bitangent");
constant(BoneWeights, "boneWeights");
constant(BoneIds, "boneIds");
}

namespace Default {
constexpr int PositionLocation = 0;
constexpr int Location = -1;
}

void InputLayoutShapeLocations::import(const JsonHelper &jsonHelper) {
    using namespace Config;

    auto read = [&](auto key, const int defaultLocation = Default::Location)
    {
        return jsonHelper.readValue(key, defaultLocation);
    };

    position = read(Position, Default::PositionLocation);
    texCoord = read(TexCoord);
    normal = read(Normal);
    tangent = read(Tangent);
    bitangent = read(Bitangent);
    boneWeights = read(BoneWeights);
    boneIds = read(BoneIds);
}

JsonHelper InputLayoutShapeLocations::dump() {
    using namespace Config;

    json config;

    auto write = [&](const string &key, int location, int defaultLocation = Default::Location)
    {
        if (location != defaultLocation) {
            config[key] = location;
        }
    };

    write(Position, position, Default::PositionLocation);
    write(TexCoord, texCoord);
    write(Normal, normal);
    write(Tangent, tangent);
    write(Bitangent, bitangent);
    write(BoneWeights, boneWeights);
    write(BoneIds, boneIds);

    return config;
}
}
