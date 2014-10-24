#version 330 core

uniform vec4 color;

uniform sampler2D myTexture;

out vec4 fragColor;

in vec2 uvC;

void main()
{
	fragColor = texture2D(myTexture, uvC).rgba;
}