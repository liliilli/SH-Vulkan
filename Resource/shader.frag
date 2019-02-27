#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 textureUv0;

layout(binding = 1) uniform sampler2D uTexture0;

layout(location = 0) out vec4 outColor;

void main() {
	vec3 rgb = texture(uTexture0, textureUv0).rgb;
    outColor = vec4(rgb * fragColor, 1.0);
}