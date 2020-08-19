#pragma once

#include "glm/glm.hpp"
#include "Shader.h"

class Light
{
public:
	Light();
	Light(glm::vec3 aColor, float aIntensity,
			glm::vec3 lightDir, float dIntensity);
	~Light();

	void UseLight(Shader& shader);
private:
	glm::vec3 m_LightColor; //s_a & s_d & s_s�� ����
	glm::vec3 m_Direction; //directional light ����
	float m_AmbientIntensity; //Ambient ���� ���� (s_a = m_LightColor * m_AmbientIntensity)
	float m_DiffuseIntensity; //Diffuse ���� ���� (s_a = m_LightColor * m_AmbientIntensity)
};