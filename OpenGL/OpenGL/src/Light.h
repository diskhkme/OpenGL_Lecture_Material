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
	glm::vec3 m_LightColor; //s_a & s_d & s_s의 색상
	glm::vec3 m_Direction; //directional light 방향
	float m_AmbientIntensity; //Ambient 세기 조절 (s_a = m_LightColor * m_AmbientIntensity)
	float m_DiffuseIntensity; //Diffuse 세기 조절 (s_a = m_LightColor * m_AmbientIntensity)
};