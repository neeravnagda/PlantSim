#version 330 core
/// @brief the vertex passed in
layout (location = 0) in vec3 inVert;
/// @brief the UV passed in
layout (location = 1) in vec2 inUV;
/// @brief the normal passed in
layout (location = 2) in vec3 inNormal;

uniform mat4 MVP;
uniform mat3 N;

out vec3 fragPos;
out vec3 fragNormal;

void main(void)
{
	//Calculate the transformed position
	gl_Position = MVP * vec4(inVert, 1.0f);
	fragPos = gl_Position.xyz;
	//Calculate the transformed normal
	fragNormal = normalize(N * inNormal);
}
