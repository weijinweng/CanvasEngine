#version 330 core

layout (location = 0) in vec2 xyPos;
layout (location = 1) in vec2 uv;

out vec2 uvC;

uniform vec4 rect;
uniform vec4 color;

void main()
{
	float xPos = xyPos.x * rect.z + rect.x;
	float yPos = xyPos.y * rect.w - rect.y;
	
	uvC = uv;
	
	gl_Position = vec4(xPos, yPos, 1.0, 1);
}