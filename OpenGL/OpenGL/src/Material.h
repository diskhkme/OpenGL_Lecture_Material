#pragma once

#include "Shader.h"

class Material
{
public:
	Material();
	Material(float sIntensity, float shine);
	~Material();

	void UseMaterial(Shader& shader);

private:
	float m_SpecularIntensity;
	float m_Shininess;
};