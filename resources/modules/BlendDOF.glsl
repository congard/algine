/**
 * Blend DOF
 * It is module, not shader
 * Add it in your shader via
 * #alp include "modules/BlendDOF.glsl"
 */

uniform sampler2D cocMap;
uniform float dofSigmaDivider;

/**
 * p1 - base image
 * p2 - blurred image
 */
vec3 blendDOF(in vec3 p1, in vec3 p2, in vec2 texCoord) {
    float k = texture(cocMap, texCoord).r / dofSigmaDivider;

    #ifndef ALGINE_DOF_SKIP_RANGE_CHECK
    if (k > 1.0)
        return p2;
    #endif
    
    return mix(p1, p2, k);
}