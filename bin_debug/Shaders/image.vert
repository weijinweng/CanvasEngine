#version 330 core

layout (location = 0) in vec2 vertexPosition_image;
layout (location = 1) in vec2 uv1;

out vec2 uv;

void main()
{
	gl_Position = vec4(vertexPosition_image,0,1);
	uv = uv1;
}