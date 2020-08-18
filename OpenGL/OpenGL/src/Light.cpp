#include "Light.h"

Light::Light()
	:m_LightColor{glm::vec3{1.0f,1.0f,1.0f}},
	m_AmbientIntensity{1.0f}
{
}

Light::Light(glm::vec3 aColor, float aIntensity)
	: m_LightColor{aColor}, m_AmbientIntensity{aIntensity}
{
}

Light::~Light()
{
}

void Light::UseLight(Shader & shader)
{
	//alpha값은 사실 사용 안함, 구조체 멤버도 일반 c코드 문법 이름으로 접근 가능
	shader.SetUniform3f("u_DirectionalLight.lightColor", m_LightColor.r, m_LightColor.g, m_LightColor.b);
	shader.SetUniform1f("u_DirectionalLight.ambientIntensity", m_AmbientIntensity);

}

