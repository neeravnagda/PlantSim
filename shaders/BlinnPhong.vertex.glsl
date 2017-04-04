#version 330 core
/// @brief The vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief The UV passed in
layout (location = 1) in vec2 inUV;
/// @brief The normal passed in
layout (location = 2) in vec3 inNormal;

#define MAX_LIGHTS 4

/// @struct LightInfo
/// @brief a structure to hold light parameters
struct LightInfo
{
	bool isActive;
	vec3 Position;//World position of the light
	vec3 La;//Ambient light intensity
	vec3 Ld;//Diffuse light intensity
	vec3 Ls;//Specular light intensity
};

uniform 	vec3 viewerPos;
/// @brief model matrix
uniform mat4 M;
/// @brief model * view matrix
uniform mat4 MV;
/// @brief model * view * projection matrix
uniform mat4 MVP;
/// @brief normal matrix
uniform mat3 N;
/// @brief An array of lights
uniform LightInfo Lights[MAX_LIGHTS];

/// @brief fragment position
out vec3 fragPos;
/// @brief fragment normal
out vec3 fragNormal;
/// @brief eye direction
out vec3 eyeDirection;
/// @brief array of half-vectors
out vec3 halfVectors[MAX_LIGHTS];
/// @brief array of light directions
out vec3 lightDirections[MAX_LIGHTS];

void main(void)
{
	vec4 worldPosition = M * vec4(inVert, 1.0f);
	eyeDirection = normalize(viewerPos - worldPosition.xyz);

	//Calculate the vertex position
	gl_Position = MVP * vec4(inVert, 1.0f);
	//Calculate the fragment position
	vec4 eyePos = MV * vec4(inVert,1.0f);
	fragPos = eyePos.xyz / eyePos.w;
	//Calculate the fragment normal
	fragNormal = normalize(N * inNormal);

	//Calculate half-vectors and light directions
	for (int i=0; i<MAX_LIGHTS; ++i)
	{
		if (Lights[i].isActive)
		{
			lightDirections[i] = normalize(vec3(Lights[i].Position - eyePos.xyz));
			halfVectors[i] = normalize(eyeDirection + lightDirections[i]);
		}
	}
}
