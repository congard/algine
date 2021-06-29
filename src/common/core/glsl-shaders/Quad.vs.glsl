layout (location = 0) in vec3 inPos;
out vec2 texCoord;

void main() {
    texCoord = inPos.xy * 0.5f + 0.5f;
    gl_Position = vec4(inPos, 1.0f);
}
