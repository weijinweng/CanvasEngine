#ifndef CVS_LIGHT_H
#define CVS_LIGHT_H

#include "CVS_Precompiled.h"
#include "CVS_Transform.h"

struct CVS_LightProperties{
	CVS_Transform transform;
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
};

struct CVS_Light{
	CVS_LightProperties properties;
	CVS_Light();
	void DirectionalLight(cvec3 lightDirection);
};


#endif