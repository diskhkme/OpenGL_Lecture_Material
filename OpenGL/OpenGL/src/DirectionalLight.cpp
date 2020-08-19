#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
	: Light{}, m_Direction{glm::vec3{0.0f, -1.0f, 0.0f}}
{
}

DirectionalLight::DirectionalLight(glm::vec3 aColor, float aIntensity, glm::vec3 direction, float dIntensity)
	:Light{aColor,aIntensity, dIntensity}, m_Direction{direction}
{
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::UseLight(Shader & shader)
{
	shader.SetUniform3f("u_DirectionalLight.base.lightColor", m_LightColor.r, m_LightColor.g, m_LightColor.b);
	shader.SetUniform1f("u_DirectionalLight.base.ambientIntensity", m_AmbientIntensity);
	shader.SetUniform1f("u_DirectionalLight.base.diffuseIntensity", m_DiffuseIntensity);

	shader.SetUniform3f("u_DirectionalLight.direction", m_Direction.x, m_Direction.y, m_Direction.z);
	
}

