#version 330

layout(location = 0) in vec3 vert_pos;

uniform mat4 MVP;

void main(void)
{
	gl_Position = MVP *  vec4(vert_pos, 1);
}