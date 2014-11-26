#include "Canvas.h"

CVS_LightProperties::CVS_LightProperties() :position(0, 0, 0, 1.0)
, ambient(1.0, 1.0, 1.0, 1.0), diffuse(1.0f, 1.0f, 1.0f, 1.0f),
specular(1.0, 1.0, 1.0, 1.0), constAttenuation(0.0f), linearAttenuation(1.0f),
quadraticAttenuation(0.0f), type(CVS_LGT_PT), spotCosCutoff(1.0f), rad(1.0f)
{}

void CVS_LightProperties::CalculateBoundRadius()
{
	//Use the quadratic formula to determine our bound radius. 
	//It is expected that at attenuation 1.0f/255.0f, the color
	//become indistingqushable.
	float val = 0.0;
	if (quadraticAttenuation > 0.00001f)
	{
		val = (((-linearAttenuation) + sqrt((linearAttenuation)*(linearAttenuation)-(4 * (constAttenuation - 255) * quadraticAttenuation))))
			/ (2 * quadraticAttenuation);
		printf("Lol ret %f\n", val);
	}
	else {
		val = ( 255.0f - constAttenuation) / linearAttenuation;
		printf("Why ret %f\n", val);
	}

	printf("Att ret %f\n", val);
	rad = val;
}

CVS_Light::CVS_Light()
{
	this->properties.CalculateBoundRadius();
	printf("rad of light is %f\n", properties.rad);
}

void CVS_Light::SetPosition(cvec3 pos)
{
	properties.position = cvec4(pos,1.0f);
	properties.transform.translation = pos;
	properties.transform.calculateMatrix();
}

void CVS_Light::DirectionalLight(glm::vec3 direction)
{
	properties.position = cvec4(direction, 1.0f);
	properties.type = CVS_LightProperties::CVS_LGT_DIR;
	properties.constAttenuation = 1.0f;
	properties.linearAttenuation = 0.0f;
	properties.quadraticAttenuation = 0.0f;
}

void CVS_Light::PointLight(glm::vec3 pos)
{
	properties.position = cvec4(pos, 1.0f);
	properties.type = CVS_LightProperties::CVS_LGT_PT;
}