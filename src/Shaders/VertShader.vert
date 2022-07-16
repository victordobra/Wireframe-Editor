#version 450

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 uvCoord;
layout(location = 2) in vec4 color;

layout(location = 0) out vec2 fragUvCoord;
layout(location = 1) out vec4 fragColor;

layout(push_constant) uniform Push {
    vec2 scale;
} push;

void main() {
    gl_Position = vec4(position * push.scale - 1.0, 0.0, 1.0);
    fragUvCoord = uvCoord;
    fragColor = color;
}