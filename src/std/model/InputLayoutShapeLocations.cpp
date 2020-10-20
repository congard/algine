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

    inPosition = read(Position, Default::PositionLocation);
    inTexCoord = read(TexCoord);
    inNormal = read(Normal);
    inTangent = read(Tangent);
    inBitangent = read(Bitangent);
    inBoneWeights = read(BoneWeights);
    inBoneIds = read(BoneIds);
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

    write(Position, inPosition, Default::PositionLocation);
    write(TexCoord, inTexCoord);
    write(Normal, inNormal);
    write(Tangent, inTangent);
    write(Bitangent, inBitangent);
    write(BoneWeights, inBoneWeights);
    write(BoneIds, inBoneIds);

    return config;
}
}
