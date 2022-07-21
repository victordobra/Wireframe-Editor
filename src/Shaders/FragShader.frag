#version 450

layout(location = 0) in vec2 fragUvCoord;
layout(location = 1) in vec4 fragColor;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform sampler2D fontTexture;

void main() {
    if(texture(fontTexture, fragUvCoord).w == 0.0)
        discard;
    outColor = fragColor * texture(fontTexture, fragUvCoord);
}