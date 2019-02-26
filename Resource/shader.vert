#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inBaseColor;
layout(location = 0) out vec3 fragColor;

layout(binding = 0) uniform DyUniformBufferObject 
{
	mat4 uModel;
	mat4 uView;
	mat4 uProj;
} uniUbo;

mat4 DyGetPVM() { return uniUbo.uProj * uniUbo.uView * uniUbo.uModel; }

void main() 
{
    gl_Position = DyGetPVM() * vec4(inPosition, 1.0);
    fragColor = inBaseColor;
}