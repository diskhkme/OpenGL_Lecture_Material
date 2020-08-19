#include "Light.h"

Light::Light()
	:m_LightColor{glm::vec3{1.0f,1.0f,1.0f}},
	m_AmbientIntensity{0.2f},
	m_DiffuseIntensity{1.0f}
{
}

Light::Light(int shadowWidth, int shadowHeight, glm::vec3 aColor, float aIntensity, float dIntensity)
	: m_LightColor{ aColor }, m_AmbientIntensity{ aIntensity },
	m_DiffuseIntensity{ dIntensity }
{
	//Light °´Ã¼°¡ µ¿Àû ÇÒ´çµÈ ShadowMap °´Ã¼¸¦ °¡Áü
	m_ShadowMap = new ShadowMap{shadowWidth, shadowHeight};
}


Light::~Light()
{
	delete m_ShadowMap;
}

