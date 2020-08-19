#include "Light.h"

Light::Light()
	:m_LightColor{glm::vec3{1.0f,1.0f,1.0f}},
	m_AmbientIntensity{0.2f},
	m_DiffuseIntensity{1.0f}
{
}

Light::Light(glm::vec3 aColor, float aIntensity, float dIntensity)
	: m_LightColor{ aColor }, m_AmbientIntensity{ aIntensity },
	m_DiffuseIntensity{ dIntensity }
{
}


Light::~Light()
{
}

