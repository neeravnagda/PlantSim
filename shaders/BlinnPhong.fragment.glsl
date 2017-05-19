#version 330 core
/// @brief the output colour
layout (location = 0) out vec4 fragColour;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the UV coordinates
in vec2 uvCoord;
/// @brief fragment position
in vec3 fragPos;
/// @brief fragment normal
in vec3 fragNormal;
/// @brief eye direction
in vec3 eyeDirection;
/// @brief sunlight direction
in vec3 sunDirection;
/// @brief half vector
in vec3 halfVector;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the texture
//----------------------------------------------------------------------------------------------------------------------
uniform sampler2D tex;
vec4 diffuseColour = texture(tex, uvCoord);
//----------------------------------------------------------------------------------------------------------------------
/// @struct LightInfo
/// @brief a structure to hold light parameters
//----------------------------------------------------------------------------------------------------------------------
struct LightInfo
{
	vec3 Position;//World position of the light
	vec3 La;//Ambient light intensity
	vec3 Ld;//Diffuse light intensity
	vec3 Ls;//Specular light intensity
};
//----------------------------------------------------------------------------------------------------------------------
/// @brief the sunlight
//----------------------------------------------------------------------------------------------------------------------
uniform LightInfo Sun = LightInfo(
			vec3(0.0f, 100.0f, 0.0f),
			vec3(0.5f),
			vec3(1.0f),
			vec3(1.0f));
//----------------------------------------------------------------------------------------------------------------------
/// @struct MaterialInfo
/// @brief a structure to hold material parameters
//----------------------------------------------------------------------------------------------------------------------
struct MaterialInfo
{
	vec3 Ka;//Ambient reflectivity
	vec3 Kd;//Diffuse reflectivity
	vec3 Ks;//Specular reflectivity
	float Shininess;//Specular shininess factor
};
//----------------------------------------------------------------------------------------------------------------------
/// @brief the material of this object
//----------------------------------------------------------------------------------------------------------------------
uniform MaterialInfo Material = MaterialInfo(
			vec3(0.1f, 0.1f, 0.1f),//Ambient
			vec3(0.6f, 0.6f, 0.6f),//Diffuse
			vec3(1.0f, 1.0f, 1.0f),//Specular
			100.0f//Specular shininess
			);
//----------------------------------------------------------------------------------------------------------------------
/// @brief calculate the light contribution of one light
/// @param _diffuseColour The diffuse colour from the texture
//----------------------------------------------------------------------------------------------------------------------
vec3 calculateLightContribution()
{
	float lambertTerm = dot(fragNormal, sunDirection);
	if (lambertTerm > 0)//This is the amount of diffuse contribution
	{
		//Light vector
		vec3 s = Sun.Position - fragPos;
		//Normal dot Half-vector
		float NdotH = max(dot(fragNormal, halfVector),0.0f);
		//Calculate the ambient diffuse and specular components
		vec3 ambient = Sun.La * Material.Ka;
		vec3 diffuse = Sun.Ld * Material.Kd * diffuseColour.rgb * lambertTerm;
		vec3 specular = Sun.Ls * Material.Ks * diffuseColour.rgb * pow(NdotH, Material.Shininess);

		return ambient + diffuse + specular;//The final colour
	}
	else return vec3(0,0,0);//No diffuse contribution so set to black
}
//----------------------------------------------------------------------------------------------------------------------
void main(void)
{
	if (diffuseColour.w == 0.0f)//Discard fragments with zero alpha
	{
		discard;
	}
	vec3 totalColour = calculateLightContribution();

	fragColour = vec4(totalColour,1.0f);
}
