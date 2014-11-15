#version 330
 
// Attributes per position: position and normal
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uvC;
layout (location = 2) in  vec3 normal;
 
uniform mat4   MVP;
uniform mat4   M;
uniform mat4   V;
 
smooth out vec3 vPosition;
smooth out vec3 vNormal;
smooth out vec2 uv;
 
void main(void)
{
    // Get surface normal in eye coordinates
    vNormal = (V * M * vec4(normal,0)).xyz;
 
    // Get vertex position in eye coordinates
    vec4 vertexPos = M * V * vec4(position,1);
    vec3 vertexEyePos = vertexPos.xyz / vertexPos.w;
 
    vPosition = vertexEyePos;
 
    // Don't forget to transform the geometry!
    gl_Position = MVP * vec4(position, 1);
	
	uv = uvC;
}