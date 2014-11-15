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
};
 
layout(shared) uniform Lights
{
    Light light[8];
} lights;
 
Material
{
    vec4    ambient;
    vec4    diffuse;
    vec4    specular;
    float   shininess;
} material;
 
uniform int num_lights;

uniform sampler2D myTextureSampler;
 
smooth in vec3 vPosition;
smooth in vec3 vNormal;
in vec2 uv;

vec4
pointLight (int lightID)
{
    float nDotVP;       // normal * light direction
    float nDotR;        // normal * light reflection vector
    float bt;           // blinn term
    float attenuation;  // computed attenuation factor
    float d;            // distance from surface to light position
    vec3 VP;            // direction from surface to light position
    vec3 reflection;    // direction of maximum highlight
	vec3 viewDirection;
 
    // Compute vector from surface to light position
    VP = vec3 (lights.light[lightID].position) - vPosition;
 
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
	
	vec3 position0 = vec3(0,0,0);
	viewDirection = position0 - vPosition;
	
	vec3 halfAngle = normalize(VP + viewDirection);
	bt = dot(vNormal, halfAngle);
	bt = clamp(bt,0,1);
	bt = nDotVP != 0.0 ? bt : 0.0;
	bt = pow(bt,material.shininess);
		
    vec4 ambient = material.ambient * lights.light[lightID].ambient * attenuation;
    vec4 diffuse = material.diffuse * lights.light[lightID].diffuse * nDotVP * attenuation;
    vec4 specular = material.specular * lights.light[lightID].specular * bt * attenuation;
 
    return ambient + diffuse + specular;
}
 
void main(void)
{
	material.diffuse = vec4(texture2D(myTextureSampler, uv).rgb,1);
	material.ambient = material.diffuse*vec4(0.3,0.3,0.3,1);
	material.specular = vec4(0.3,0.3,0.3,1);
	material.shininess = 10;
	
    for (int i = 0; i < num_lights; ++i)
        fragmentColor += pointLight (i);
	//fragmentColor = vec4(lights.light[0].diffuse.x);
	fragmentColor = vec4(gl_FragCoord.z);
}