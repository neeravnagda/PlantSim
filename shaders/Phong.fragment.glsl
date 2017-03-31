#version 330
/// @brief the output colour
layout (location = 0) out vec4 fragColour;

in vec3 fragPos;
in vec3 fragNormal;

/// @struct LightInfo
/// @brief a structure to hold light parameters
struct LightInfo
{
	vec3 Position;//Light position
	vec3 La;//Ambient light intensity
	vec3 Ld;//Diffuse light intensity
	vec3 Ls;//Specular light intensity
};

/// @brief the light in the scene
uniform LightInfo Light0 = LightInfo(
			vec3(2.0f, 2.0f, -10.0f),//position
			vec3(0.2f, 0.6f, 0.2f),//ambient intensity
			vec3(1.0f, 1.0f, 1.0f),//diffuse intensity
			vec3(1.0f, 1.0f, 1.0f)//specular intensity
			);

uniform LightInfo Light1 = LightInfo(
			vec3(-2.0f, -2.0f, -8.0f),//position
			vec3(0.1f, 0.1f, 0.7f),//ambient intensity
			vec3(1.0f, 1.0f, 1.0f),//diffuse intensity
			vec3(1.0f, 1.0f, 1.0f)//specular intensity
			);

struct MaterialInfo
{
	vec3 Ka;//Ambient reflectivity
	vec3 Kd;//Diffuse reflectivity
	vec3 Ks;//Specular reflectivity
	float Shininess;//Specular shininess factor
};

/// @brief the material of this object
uniform MaterialInfo Material = MaterialInfo(
			vec3(0.1f, 0.1f, 0.1f),//Ambient
			vec3(0.6f, 0.6f, 0.6f),//Diffuse
			vec3(1.0f, 1.0f, 1.0f),//Specular
			10.0f//Specular shininess
			);

//Calculate the vertex position
vec3 v = normalize(fragPos);

vec3 calculateLightContribution(in LightInfo _light)
{
	//Calculate the light vector
	vec3 s = normalize(_light.Position - fragPos);
	//Reflect the light around the vertex normal
	vec3 r = reflect(-s, fragNormal);
	//Compute the light from the ambient, diffuse and specular components
	return vec3(
				_light.La * Material.Ka +
				_light.Ld * Material.Kd * max( dot(s, fragNormal), 0.0f) +
				_light.Ls * Material.Ks * pow( max( dot(r,v), 0.0f), Material.Shininess));
}

void main(void)
{
	vec3 totalColour = calculateLightContribution(Light0) + calculateLightContribution(Light1);
	fragColour = vec4(totalColour,1.0f);
}
