#version 330 core
/// @brief The vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief The UV passed in
layout (location = 1) in vec2 inUV;
/// @brief The normal passed in
layout (location = 2) in vec3 inNormal;
//----------------------------------------------------------------------------------------------------------------------
uniform 	vec3 viewerPos;
/// @brief model matrix
uniform mat4 M;
/// @brief model * view matrix
uniform mat4 MV;
/// @brief model * view * projection matrix
uniform mat4 MVP;
/// @brief normal matrix
uniform mat3 N;
/// @brief the sun position
uniform vec3 sunPosition = vec3(0.0f, 100.0f, 0.0f);
/// @brief the texture tile factor
uniform float texScale = 1.0f;
//----------------------------------------------------------------------------------------------------------------------
/// @brief the UV coordinates
out vec2 uvCoord;
/// @brief fragment position
out vec3 fragPos;
/// @brief fragment normal
out vec3 fragNormal;
/// @brief eye direction
out vec3 eyeDirection;
/// @brief sunlight direction
out vec3 sunDirection;
/// @brief half vector
out vec3 halfVector;
//----------------------------------------------------------------------------------------------------------------------
void main(void)
{
	uvCoord = inUV * texScale;

	vec4 worldPosition = M * vec4(inVert, 1.0f);
	eyeDirection = normalize(viewerPos - worldPosition.xyz);

	//Calculate the vertex position
	gl_Position = MVP * vec4(inVert, 1.0f);
	//Calculate the fragment position
	vec4 eyePos = MV * vec4(inVert,1.0f);
	fragPos = eyePos.xyz / eyePos.w;
	//Calculate the fragment normal
	fragNormal = normalize(N * inNormal);

	//Calculate half-vector and sunlight direction
	sunDirection = normalize(vec3(sunPosition - eyePos.xyz));
	halfVector = normalize(eyeDirection + sunDirection);
}
