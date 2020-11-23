#include "DirectionalLight.h"

#include "glm/gtc/matrix_transform.hpp"

DirectionalLight::DirectionalLight()
	: Light{}, m_Direction{glm::vec3{0.0f, -1.0f, 0.0f}}
{
	//directional light�� ������ �ϳ��̹Ƿ� shadow map ������ orthographic projection �����
	//m_LightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
}

DirectionalLight::DirectionalLight(int shadowWidth, int shadowHeight,
	glm::vec3 aColor, float aIntensity, glm::vec3 direction, float dIntensity)
	:Light{shadowWidth, shadowHeight, aColor,aIntensity, dIntensity}, m_Direction{direction}
{
	//m_LightProjection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 20.0f);
	m_LightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 100.0f); //���� �ٲپ� � ȿ���� �ִ��� ����
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
	
	shader.SetUniform1i("u_DirectionalShadowMap", slot);//depth map�� ������� �� �׸��� ������ ���ؼ��� �켱 depth map�� ����� �ؽ�ó ���� ���� ����
	shader.SetUniformMat4f("u_DirectionalLightTransform", CalculateLightTransform()); //�׸��� depth map�� ������ light transform ���� ����
	//**���� depth map�� ���� �׸��� ���� ���̴��� ���� Ŀ�Կ� ����
}

void DirectionalLight::UseLightForShadow(Shader & shader)
{
	//Depth map ������ ���ؼ��� light transform ���� ����
	shader.SetUniformMat4f("u_DirectionalLightTransform", CalculateLightTransform());
}

void DirectionalLight::SetDirection(const glm::vec3 & dir)
{
	m_Direction = dir;
}

glm::mat4 DirectionalLight::CalculateLightTransform()
{
	//Light�� ���� ViewProjection Matrix
	//directional light�� ��ġ�� �������� �ʾ����Ƿ�, 
	//0,0,0 ��ġ�� ���Ƿ� ������� direction �ݴ� ���⸸ŭ ������ �ִ� ������ view matrix ����
	return m_LightProjection * glm::lookAt(-m_Direction, glm::vec3{ 0.0f,0.0f,0.0f }, glm::vec3{ 0.0f,1.0f,0.0f });
}

