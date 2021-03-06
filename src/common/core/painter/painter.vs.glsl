layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 pos;
out vec2 texCoords;

uniform mat4 projection;
uniform mat4 transformation;
uniform mat4 paintTransformation;

void main() {
    gl_Position = projection * transformation * vec4(vertex.xy, 0.0f, 1.0f);
    pos = vertex.xy;
    texCoords = (paintTransformation * vec4(vertex.zw, 0.0f, 1.0f)).xy;
}
