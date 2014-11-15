#version 330 core

layout (location = 0) out vec3 fragmentDepth;

void main()
{
	fragmentDepth = vec3(gl_FragCoord.z);
}