#pragma once

#include "glm/glm.hpp"
#include "Shader.h"

//�پ��� ������ Light�� ���� �� �����Ƿ�, light�� Base class�� �ϰ�,
//Ư�� light�� ���� Ŭ������ �����ϵ��� ��.

#include "ShadowMap.h" //����Ʈ�� shadow map�� �׸� �� ����

class Light
{
public:
	Light();
	Light(int shadowWidth, int shadowHeight,
		glm::vec3 aColor, float aIntensity, float dIntensity); //��� light�� ������ ���� ���� �ƴ�
	virtual ~Light();

	ShadowMap* GetShadowMap() { return m_ShadowMap; }
	//�Է��ؾ� �ϴ� uniform�� light ������ ���� �ٸ�
protected:
	glm::vec3 m_LightColor; //s_a & s_d & s_s�� ����

	float m_AmbientIntensity; //Ambient ���� ���� (s_a = m_LightColor * m_AmbientIntensity)
	float m_DiffuseIntensity; //Diffuse ���� ���� (s_a = m_LightColor * m_AmbientIntensity)

	glm::mat4 m_LightProjection; //shadow map ����� ���� ��ȯ ���
	ShadowMap* m_ShadowMap; //shadow map ��ü
};