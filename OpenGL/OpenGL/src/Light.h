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
	glm::vec3 m_LightColor; //color of s_a & s_d &s_s
	glm::vec3 m_Direction;
	float m_AmbientIntensity; 
	float m_DiffuseIntensity;
};