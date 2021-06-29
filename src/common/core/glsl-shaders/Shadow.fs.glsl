#ifdef ALGINE_SHADOW_MAPPING_TYPE_POINT_LIGHTING
in vec4 fragPos; // world space

uniform vec3 lightPos; // world space
uniform float farPlane; // shadow matrix far plane

void main() {
    float lightDistance = length(fragPos.xyz - lightPos);

    // map to [0; 1] range by dividing by farPlane
    lightDistance = lightDistance / farPlane;

    // write this as modified depth
    gl_FragDepth = lightDistance;
}
#else
void main() {
    // empty
}
#endif
