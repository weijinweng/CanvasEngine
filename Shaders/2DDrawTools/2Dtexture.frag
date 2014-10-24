#version 330 core

uniform vec4 color;

out vec4 fragColor;

in vec2 uvC;

uniform sampler2D colorTexture;

void main()
{
	fragColor = texture(colorTexture, uvC).rgba;
}