#version 330

#alp include <ToneMapping/exposure>
#alp include <Blending/screen>
#alp include "modules/BlendDOF.glsl"

out vec3 fragColor;
in vec2 texCoord;

uniform sampler2D bloom;
uniform sampler2D dof;
uniform sampler2D image;
uniform float exposure;
uniform float gamma;

void main() {
    vec3 color = blendDOF(
        texture(image, texCoord).rgb,
        texture(dof, texCoord).rgb,
        texCoord
    );
    
    color = blendScreen(texture(bloom, texCoord).rgb, color);
    
    fragColor = tonemapExposure(color, exposure);
    fragColor = pow(fragColor, vec3(1.0f / gamma)); // gamma correction
}
