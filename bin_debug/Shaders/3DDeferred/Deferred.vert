#version 330 core

layout (location = 0) vec3 vert_ms;
layout (location = 1) vec2 uv_ms;
layout (location = 2) vec3 normal_ms;

out vec3 normal_ws;
out vec2 uv;
out vec3 pos_ws;

uniform mat4 MVP;
uniform mat4 M;

void main(){
	pos_ws = (M * vec4(vert_ms, 1.0f)).xyz;
	gl_Position = MVP * vec4(vert_ms, 1.0f);
	
	normal_ws = (M * vec4(normal_ms,1.0f)).xyz;
	uv = uv_ms;
 
}