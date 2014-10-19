#version 330 core

uniform vec4 color;

out vec4 fragColor;

uniform bool textBool;
uniform sampler2D fTexture;

in vec2 uvC;

void main()
{
	if(!textBool)
		fragColor = color;
	else fragColor = texture(fTexture, uvC).rgba;
}