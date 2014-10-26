#version 330

in vec3 normal_ws;
in vec2 uv;
in vec3 pos_ws;

layout (location = 0) out vec3 worldPos;
layout (location = 1) out vec3 diffuse;
layout (location = 2) out vec3 normal;
layout (location = 3) out vec3 texCoord;

uniform sampler2D diffuseColorMap;

void main()
{
	worldPos = pos_ws;
	diffuse = texture(diffuseColorMap, uv).rgb;
	texCoord = vec3(uv,0.0f);
	normal = normalize(normal_ws);
}