#pragma once

#include "Light.h"

//Directional Light 유도 클래스
class DirectionalLight : public Light
{
public:
	DirectionalLight();
	DirectionalLight(glm::vec3 aColor, float aIntensity,
					glm::vec3 direction, float dIntensity);
	~DirectionalLight();

	void UseLight(Shader& shader);
private:
	glm::vec3 m_Direction;
};