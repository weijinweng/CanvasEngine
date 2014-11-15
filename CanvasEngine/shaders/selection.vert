#version 330 

layout (location = 0) in vec3 Pos_ws;
layout (location = 1) in vec2 uv;
layout ( location = 2) in vec3 normal;

uniform mat4 MVP;

void main(void)
{
	gl_Position = MVP * vec4(Pos_ws.x, Pos_ws.y, Pos_ws.z, 1.0f);
}