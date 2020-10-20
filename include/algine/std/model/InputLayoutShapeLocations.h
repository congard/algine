#ifndef ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
#define ALGINE_INPUTLAYOUTSHAPELOCATIONS_H

#include <algine/core/transfer/Transferable.h>

namespace algine {
class InputLayoutShapeLocations: public Transferable {
public:
    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

public:
    int inPosition = 0;
    int inTexCoord = -1;
    int inNormal = -1;
    int inTangent = -1;
    int inBitangent = -1;
    int inBoneWeights = -1;
    int inBoneIds = -1;
};
}

#endif //ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
