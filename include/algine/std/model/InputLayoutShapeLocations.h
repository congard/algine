#ifndef ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
#define ALGINE_INPUTLAYOUTSHAPELOCATIONS_H

#include <algine/core/transfer/Transferable.h>

namespace algine {
class InputLayoutShapeLocations: public Transferable {
public:
    void import(const JsonHelper &jsonHelper) override;
    JsonHelper dump() override;

public:
    int position = 0;
    int texCoord = -1;
    int normal = -1;
    int tangent = -1;
    int bitangent = -1;
    int boneWeights = -1;
    int boneIds = -1;
};
}

#endif //ALGINE_INPUTLAYOUTSHAPELOCATIONS_H
