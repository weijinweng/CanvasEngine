#version 330 core
 
out vec4 fragmentColor;
 
struct Light
{
    vec4    position;
    vec4    ambient;
    vec4    diffuse;
    vec4    specular;
    float   constant_attenuation;
    float   linear_attenuation;
    float   quadratic_attenuation;
    vec3    spot_direction;
    float   spot_cutoff;
    float   spot_exponent;
	int type;
};
 
layout(shared) uniform Lights
{
    Light light[10];
} lights;
 
struct Material
{
    vec4    diffuse;
} material;
 
uniform int num_lights;

uniform sampler2D myTextureSampler;
 
uniform sampler2DShadow shadowTextures;

uniform mat4 DepthBiasMVP;

smooth in vec3 wPosition;
smooth in vec3 wNormal;
smooth in vec2 uv;
smooth in vec3 mPosition;

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float directionLight(int lightID)
{
	vec3 lightDir = lights.light[lightID].position.xyz;
	lightDir = normalize(lightDir);
	float intensity = dot(lightDir, wNormal);
	float bias = 0.05;
	float visibility = 1.0;
	vec4 ShadowCoord = (DepthBiasMVP * vec4(mPosition, 1));
	if(ShadowCoord.z < 1.0f)
	{
		for(int i = 0; i < 16 ; ++i)
		{
			int index = i;
			visibility -= 0.2*(1.0-texture( shadowTextures, vec3(ShadowCoord.xy + poissonDisk[index]/1000.0, (ShadowCoord.z - bias)/ShadowCoord.w) ));
		}
	}
	
	return visibility * intensity;
}

float calcIntensity(int lightID)
{
	switch (lights.light[lightID].type)
	{
		case 0:
			return 0.0f;
		case 2: 
			return directionLight(lightID);
	}
	return 0.0f;
}


void main(void)
{
	material.diffuse = vec4(texture(myTextureSampler, uv).rgb,1);
	float multiplier = 1.0f;
	float intensity = 0.0;
	for(int i = 0; i < num_lights; i++)
	{
		intensity += calcIntensity(i);
	}
	
	if (intensity > 0.55)
		multiplier = 1.0f;
	else if (intensity > 0.25)
		multiplier = 0.7f;
	else if (intensity > 0.15)
		multiplier = 0.3f;
	else
		multiplier = 0.1f;
	fragmentColor = multiplier * material.diffuse;
	//fragmentColor = vec4(wNormal,1);
}






















