#version 330 

uniform int DrawIndex;
uniform int ObjectIndex;

out vec3 fragColor;

void main(void)
{
	fragColor = vec3(float(ObjectIndex), float(DrawIndex), float(gl_PrimitiveID + 1));
}