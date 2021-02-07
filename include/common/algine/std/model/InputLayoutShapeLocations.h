#ifndef ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
#define ALGINE_INPUTLAYOUTSHAPELOCATIONS_H

namespace algine {
class InputLayoutShapeLocations {
public:
    constexpr static int None = -1;

public:
    int position = None;
    int texCoord = None;
    int normal = None;
    int tangent = None;
    int bitangent = None;
    int boneWeights = None;
    int boneIds = None;
};
}

#endif //ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
