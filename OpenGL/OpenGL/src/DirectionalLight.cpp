#include "DirectionalLight.h"

#include "glm/gtc/matrix_transform.hpp"

DirectionalLight::DirectionalLight()
	: Light{}, m_Direction{glm::vec3{0.0f, -1.0f, 0.0f}}
{
	//directional light는 방향이 하나이므로 shadow map 생성에 orthographic projection 사용함
	//m_LightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

DirectionalLight::DirectionalLight(int shadowWidth, int shadowHeight,
	glm::vec3 aColor, float aIntensity, glm::vec3 direction, float dIntensity)
	:Light{shadowWidth, shadowHeight, aColor,aIntensity, dIntensity}, m_Direction{direction}
{
	//m_LightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
	m_LightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f); //값을 바꾸어 어떤 효과가 있는지 보자
}

DirectionalLight::~DirectionalLight()
{
}

void DirectionalLight::UseLight(Shader & shader, int slot)
{
	shader.SetUniform3f("u_DirectionalLight.base.lightColor", m_LightColor.r, m_LightColor.g, m_LightColor.b);
	shader.SetUniform1f("u_DirectionalLight.base.ambientIntensity", m_AmbientIntensity);
	shader.SetUniform1f("u_DirectionalLight.base.diffuseIntensity", m_DiffuseIntensity);

	shader.SetUniform3f("u_DirectionalLight.direction", m_Direction.x, m_Direction.y, m_Direction.z);
	
	shader.SetUniform1i("u_DirectionalShadowMap", slot);//depth map을 기반으로 한 그림자 생성을 위해서는 우선 depth map이 저장된 텍스처 슬롯 정보 전달
	shader.SetUniformMat4f("u_DirectionalLightTransform", CalculateLightTransform()); //그리고 depth map이 생성된 light transform 정보 전달
	//**실제 depth map을 통한 그림자 생성 셰이더는 다음 커밋에 있음
}

void DirectionalLight::UseLightForShadow(Shader & shader)
{
	//Depth map 생성을 위해서는 light transform 정보 전달
	shader.SetUniformMat4f("u_DirectionalLightTransform", CalculateLightTransform());
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	//Light에 대한 ViewProjection Matrix
	//directional light는 위치가 정해지지 않았으므로, 
	//0,0,0 위치를 임의로 기준잡고 direction 반대 방향만큼 떨어져 있는 것으로 view matrix 만듬
	return m_LightProjection * glm::lookAt(-m_Direction, glm::vec3{ 0.0f,0.0f,0.0f }, glm::vec3{ 0.0f,1.0f,0.0f });
}

