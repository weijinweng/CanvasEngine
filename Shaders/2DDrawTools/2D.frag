#version 330 core

uniform vec4 color;

out vec4 fragColor;

in vec2 uvC;

void main()
{
	fragColor = color;
}