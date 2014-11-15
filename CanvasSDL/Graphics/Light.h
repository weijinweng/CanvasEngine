#pragma once
#include "Canvas.h"
#include "TransformNode.h"
namespace Canvas
{
	struct LightProperties
	{
		transformNode node;
		glm::vec4 position;
		glm::vec4 ambient;
		glm::vec4 diffuse;
		glm::vec4 specular;
		float constAttenuation;
		float linearAttentuation;
		float quadraticAttentuation;
		int type;
		glm::vec3 spotDirection;
		float spotExponent;
		float spotCosCutoff;

		LightProperties(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float constAtten, float linAtten, float quadAtten);
	};

	struct Light
	{
		Light();
		LightProperties properties;
		GLuint shadowTexture;
		bool shadow;
		std::string name;
		glm::mat4 depthVP;
		bool active;
		void directionalLight();
		void directionalLight(glm::vec3 position);
		void spotLight(glm::vec3 position, glm::vec3 target, float cosCutOff);
		void spotLight(float cosCutOff);
		void changeAttenuation(int constAtten, int linAtten, int quadAtten);
		void changeDiffuse(glm::vec4 diffuse);
		void changeAmbient(glm::vec4 ambient);
		void changeSpecular(glm::vec4 specular);
		void bufferData(char* buffer, int* offset);
	};
}