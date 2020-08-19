#pragma once

#include "glm/glm.hpp"
#include "Shader.h"

//�پ��� ������ Light�� ���� �� �����Ƿ�, light�� Base class�� �ϰ�,
//Ư�� light�� ���� Ŭ������ �����ϵ��� ��.

class Light
{
public:
	Light();
	Light(glm::vec3 aColor, float aIntensity, float dIntensity); //��� light�� ������ ���� ���� �ƴ�
	~Light();

	//�Է��ؾ� �ϴ� uniform�� light ������ ���� �ٸ�
protected:
	glm::vec3 m_LightColor; //s_a & s_d & s_s�� ����

	float m_AmbientIntensity; //Ambient ���� ���� (s_a = m_LightColor * m_AmbientIntensity)
	float m_DiffuseIntensity; //Diffuse ���� ���� (s_a = m_LightColor * m_AmbientIntensity)
};