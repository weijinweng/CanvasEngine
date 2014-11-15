#version 330 core

in vec2 uv;

uniform sampler2D myTexture;

out vec3 color;

void main()
{	
	color = vec3(texture2D(myTexture,uv).rgb);
}