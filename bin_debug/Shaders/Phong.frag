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
    vec4    ambient;
    vec4    diffuse;
    vec4    specular;
    float   shininess;
} material;
 
uniform int num_lights;

uniform sampler2D myTextureSampler;
 
uniform sampler2DShadow shadowTextures;

uniform mat4 DepthBiasMVP;
 
uniform mat4 V;
smooth in vec3 vPosition;
smooth in vec3 vNormal;
in vec2 uv;
in vec3 mPosition;
in vec3 normals;


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



vec4 pointLight (int lightID)
{
    float nDotVP;       // normal * light direction
    float nDotR;        // normal * light reflection vector
    float pf;           // power factor
    float attenuation;  // computed attenuation factor
    float d;            // distance from surface to light position
    vec3 VP;            // direction from surface to light position
    vec3 reflection;    // direction of maximum highlights
	float visibility = 1.0f;
	float bias = 0.05f;
	vec4 ShadowCoord = (DepthBiasMVP * vec4(mPosition, 1));
	
	for(int i = 0; i < 16; ++i)
	{
		int index = i;
		visibility -= 0.2*(1.0-texture( shadowTextures, vec3(ShadowCoord.xy + poissonDisk[index]/1000.0, (ShadowCoord.z - bias)/ShadowCoord.w) ));
	}
	
    // Compute vector from surface to light position
    VP = ( V * (lights.light[lightID].position)).xyz - vPosition;
	
    // Compute distance between surface and light position
    d = length (VP);
 
    // Normalize the vector from surface to light position
    VP = normalize (VP);
 
    // Compute attenuation
    attenuation = 1.f / (lights.light[lightID].constant_attenuation +
                         lights.light[lightID].linear_attenuation * d +
                         lights.light[lightID].quadratic_attenuation * d * d);
 
    reflection = normalize (reflect (-normalize (VP), normalize
                (vNormal)));
 
    nDotVP = max (0.f, dot (vNormal, VP));
    nDotR = max (0.f, dot (normalize (vNormal), reflection));
 
    if (nDotVP == 0.f)
        pf = 0.f;
    else
        pf = pow (nDotR, material.shininess);
 
    vec4 ambient = visibility * material.ambient * lights.light[lightID].ambient * attenuation;
    vec4 diffuse = visibility* material.diffuse * lights.light[lightID].diffuse * nDotVP * attenuation;
    vec4 specular = visibility* material.specular * lights.light[lightID].specular * pf * attenuation;
 
    return ambient + diffuse + specular;
}
 
void main(void)
{
	material.diffuse = vec4(texture2D(myTextureSampler, uv).rgb,1);
	material.ambient = material.diffuse * vec4(1.0,1.0,1.0,1);
	material.specular = vec4(0.3,0.3,0.3,1);
	material.shininess = 10;
	
    for (int i = 0; i < num_lights; ++i)
        fragmentColor += pointLight (i);
	
}