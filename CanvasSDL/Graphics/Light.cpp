#include "Light.h"

namespace Canvas
{
	LightProperties::LightProperties(glm::vec4 position, glm::vec4 ambient, glm::vec4 diffuse, glm::vec4 specular, float constAtten, float linAtten, float quadAtten) :position(position), ambient(ambient), diffuse(diffuse), specular(specular), constAttenuation(constAtten), linearAttentuation(linAtten), quadraticAttentuation(quadAtten)
	{
		type = -1;

	}

	Light::Light() :active(false), shadow(false), properties(glm::vec4(0.0f), glm::vec4(1.0f), glm::vec4(1.0f), glm::vec4(1.0f), 0.0f, 0.0f, 1.0f)
	{


	}

	void Light::spotLight(glm::vec3 position, glm::vec3 target, float cosCutoff)
	{
		properties.position = glm::vec4(position, 1);
		glm::vec3 direction = target - position;
		direction = glm::normalize(direction);
		properties.spotDirection = direction;
		properties.spotCosCutoff = cosCutoff;
		properties.type = 1;
	}

	void Light::directionalLight(glm::vec3 direction)
	{
		properties.position = glm::vec4(direction, 1);
		properties.type = 2;
	}

	void Light::bufferData(char* buffer, int* offsets)
	{
		int offset;
		offset = offsets[0];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*>(&buffer[0] + offset)) =
				properties.position[i];
			offset += sizeof(GLfloat);
		}

		offset = offsets[1];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*>(&buffer[0] + offset)) =
				properties.ambient[i];
			offset += sizeof(GLfloat);
		}

		offset = offsets[2];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*>(&buffer[0] + offset)) =
				properties.diffuse[i];
			offset += sizeof(GLfloat);
		}

		offset = offsets[3];
		for (int i = 0; i < 4; ++i)
		{
			*(reinterpret_cast<float*>(&buffer[0] + offset)) =
				properties.specular[i];
			offset += sizeof(GLfloat);
		}

		offset = offsets[4];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
			properties.constAttenuation;

		offset = offsets[5];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
			properties.linearAttentuation;

		offset = offsets[6];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
			properties.quadraticAttentuation;

		offset = offsets[7];
		for (int i = 0; i < 3; ++i)
		{
			*(reinterpret_cast<float*> (&buffer[0] + offset)) =
				properties.spotDirection[i];
			offset += sizeof (GLfloat);
		}

		offset = offsets[8];
		*(reinterpret_cast<float*> (&buffer[0] + offset)) =
			properties.spotCosCutoff;

		offset = offsets[9];
		*(reinterpret_cast<int*> (&buffer[0] + offset)) =
			properties.type;
	}
}