#version 330 core

uniform vec4 color;

uniform sampler2D myTexture;

out vec4 fragColor;

in vec2 uvC;

void main()
{
	vec4 textColor = texture2D(myTexture, uvC).rgba;
	fragColor = vec4(color.rgb, textColor.r);
}