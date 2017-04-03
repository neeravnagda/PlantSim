#version 330
/// @brief the output colour
layout (location = 0) out vec4 fragColour;

#define MAX_LIGHTS 4
/// @brief fragment position
in vec3 fragPos;
/// @brief fragment normal
in vec3 fragNormal;
/// @brief eye direction
in vec3 eyeDirection;
/// @brief array of half-vectors
in vec3 halfVectors[MAX_LIGHTS];
/// @brief array of light directions
in vec3 lightDirections[MAX_LIGHTS];

/// @struct LightInfo
/// @brief a structure to hold light parameters
struct LightInfo
{
	bool isActive;
	vec3 La;//Ambient light intensity
	vec3 Ld;//Diffuse light intensity
	vec3 Ls;//Specular light intensity
};

/// @brief An array of light positions
uniform vec3 LightPositions[MAX_LIGHTS];

/// @brief the lights in the scene
uniform LightInfo Lights[MAX_LIGHTS];

/// @struct MaterialInfo
/// @brief a structure to hold material parameters
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

/// @brief calculate the light contribution of one light
vec3 calculateLightContribution(in int _i)
{
	float lambertTerm = dot(fragNormal, lightDirections[_i]);
	if (lambertTerm > 0)
	{
		//Light vector
		vec3 s = LightPositions[_i] - fragPos;
		//Normal dot Half-vector
		float NdotH = max(dot(fragNormal, halfVectors[_i]),0.0f);

		return vec3(
					Lights[_i].La * Material.Ka +//Ambient contribution
					Lights[_i].Ld * Material.Kd * lambertTerm +//Diffuse contribution
					Lights[_i].Ls * Material.Ks * pow(NdotH, Material.Shininess)//Specular contribution
					);
	}
	else return vec3(0,0,0);
}

void main(void)
{
	vec3 totalColour = vec3(0,0,0);
	for (int i=0; i<MAX_LIGHTS; ++i)
	{
		if (Lights[i].isActive) totalColour += calculateLightContribution(i);
	}

	fragColour = vec4(totalColour,1.0f);
}
