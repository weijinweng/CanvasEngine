#version 330
 
// Attributes per position: position and normal
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvC;
layout (location = 2) in  vec3 normal;
 
uniform mat4   MVP;
uniform mat4   M;
uniform mat4   V;

smooth out vec3 wPosition;
smooth out vec3 wNormal;
smooth out vec2 uv;
smooth out vec3 mPosition;

void main(void)
{
	wNormal = (M * vec4(normal,1)).xyz;
	
	gl_Position = MVP * vec4(position, 1);
	
	wPosition = (M * vec4(position,1)).xyz;
	
	uv = uvC;
	mPosition = position;
}