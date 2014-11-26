#ifndef CVS_LIGHT_H
#define CVS_LIGHT_H

#include "CVS_Precompiled.h"
#include "CVS_Transform.h"

struct CVS_LightProperties{
	CVS_Transform transform;

	enum LGTEnum{
		CVS_LGT_PT,
		CVS_LGT_SPOT,
		CVS_LGT_DIR
	};

	glm::vec4 position;
	glm::vec4 ambient;
	glm::vec4 diffuse;
	glm::vec4 specular;

	float constAttenuation;
	float linearAttenuation;
	float quadraticAttenuation;

	int type;
	glm::vec3 spotDirection;
	float spotExponent;
	float spotCosCutoff;

	float rad;

	CVS_LightProperties();
	void CalculateBoundRadius();
};

struct CVS_Light{
	CVS_LightProperties properties;
	CVS_Light();
	void DirectionalLight(cvec3 lightDirection);
	void PointLight(cvec3 lightpos);
	void SetPosition(cvec3 position);
	void SetRotation(cvec3 angles);
};


#endif