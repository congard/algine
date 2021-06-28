#ifndef ALGINE_MODULE_DOF_LINEAR_COC
#define ALGINE_MODULE_DOF_LINEAR_COC

float linearCoC(float depth, float focalDepth, float focalRange) {
    float sigma = (-focalDepth + depth) / focalRange;
    return -sigma;
}

#endif //ALGINE_MODULE_DOF_LINEAR_COC
