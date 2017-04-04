#ifndef LIGHT_H_
#define LIGHT_H_

#include <ngl/Vec3.h>

//----------------------------------------------------------------------------------------------------------------------
/// @file Light.h
/// @brief This struct contains light information
/// @author Neerav Nagda
/// @version 1.0
/// @date 04/04/17
/// @struct Light
/// @brief Contains light information used by the shaders
//----------------------------------------------------------------------------------------------------------------------
typedef struct Light
{
		bool m_isActive = false;///< Check if the light is active
		ngl::Vec3 m_position = ngl::Vec3::zero();///< Position of the light
		ngl::Vec3 m_ambient = ngl::Vec3(0.5f,0.5f,0.5f);///< Ambient colour
		ngl::Vec3 m_diffuse = ngl::Vec3(1.0f,1.0f,1.0f);///< Diffuse colour
		ngl::Vec3 m_specular = ngl::Vec3(1.0f,1.0f,1.0f);///< Specular colour
} Light;

#endif // LIGHT_H_
