#version 450

vec2 vertices[6] = vec2[6] (
    vec2(-0.5, -0.5),
    vec2(0.5, -0.5),
    vec2(-0.5, 0.5),
    vec2(0.5, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

void main() {
    gl_Position = vec4(vertices[gl_VertexIndex], 0.0, 1.0);
}