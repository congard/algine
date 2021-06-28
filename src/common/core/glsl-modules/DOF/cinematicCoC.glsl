#ifndef ALGINE_MODULE_DOF_CINEMATIC_COC
#define ALGINE_MODULE_DOF_CINEMATIC_COC

float cinematicCoC(float depth, float planeInFocus, float aperture, float imageDistance) {
    float p = -planeInFocus;
    float f = (p + imageDistance) / (p * imageDistance);
    float d = -depth;
    float sigma = (aperture * f * (p - d)) / (d * (p - f));
    return -sigma;
}

#endif //ALGINE_MODULE_DOF_CINEMATIC_COC
