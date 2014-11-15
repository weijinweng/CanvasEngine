#version 330 core

layout(location = 0) in vec3 vertexPosition_modelSpace;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

uniform mat4 depthMVP;

void main()
{
	gl_Position = depthMVP * vec4(vertexPosition_modelSpace,1);
}