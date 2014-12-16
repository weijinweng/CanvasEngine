#version 330 core

layout (location = 0) in vec3  vertexPosition_modelspace;
layout (location = 1) in vec2  uvC;
layout (location = 2) in vec3  normal;
layout (location = 3) in ivec4 BoneIDs_4;
layout (location = 4) in ivec2 BoneIDs_2;
layout (location = 5) in vec4  Weights_4;
layout (location = 6) in vec2  Weights_2;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;

const int MAX_BONES = 100;
uniform mat4 gBones[MAX_BONES];

out vec2 uv;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

void main(){
	mat4 BoneTransform = gBones[BoneIDs_4[0]] * Weights_4[0];
	BoneTransform     += gBones[BoneIDs_4[1]] * Weights_4[1];
	BoneTransform     += gBones[BoneIDs_4[2]] * Weights_4[2];
	BoneTransform     += gBones[BoneIDs_4[3]] * Weights_4[3];
	BoneTransform     += gBones[BoneIDs_2[0]] * Weights_2[0];
	BoneTransform     += gBones[BoneIDs_2[1]] * Weights_2[1];

	gl_Position = MVP * BoneTransform * vec4(vertexPosition_modelspace,1);
	Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;
	vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace, 1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	Normal_cameraspace = ( V*M * BoneTransform * vec4(normal, 1)).xyz;
	uv = uvC;
}


/*
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout (location = 1) in vec2 uvC;
layout (location = 2) in vec3 normal;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

const int MAX_BONES = 100;

uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPosition_worldspace;
//uniform mat4 gBones[MAX_BONES];

out vec2 uv;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

void main(){
    mat4 BoneTransform = mat4();
	//gBones[BoneIDs[0]] * Weights[0];
    //BoneTransform     += gBones[BoneIDs[1]] * Weights[1];
    //BoneTransform     += gBones[BoneIDs[2]] * Weights[2];
    //BoneTransform     += gBones[BoneIDs[3]] * Weights[3];

	gl_Position =  MVP * vec4(vertexPosition_modelspace,1);

	Position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
	
	vec3 vertexPosition_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
	
	EyeDirection_cameraspace = vec3(0,0,0) - vertexPosition_cameraspace;

	vec3 LightPosition_cameraspace = ( V * vec4(LightPosition_worldspace, 1)).xyz;
	LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
	
	Normal_cameraspace = ( V*M * vec4(normal, 0)).xyz;		

	uv = uvC;
}
*/