layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 pos;
out vec2 rawTexCoords;
out vec2 texCoords;

uniform mat4 tpMatrix; // projection * transformation
uniform mat4 paintTransformation;

void main() {
    gl_Position = tpMatrix * vec4(vertex.xy, 0.0f, 1.0f);
    pos = vertex.xy;
    rawTexCoords = vertex.zw;
    texCoords = (paintTransformation * vec4(rawTexCoords, 0.0f, 1.0f)).xy;
}
