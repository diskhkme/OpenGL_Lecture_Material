#include "Material.h"

Material::Material()
	:m_SpecularIntensity{ 0.0f }, m_Shininess{0.0f}
{
}

Material::Material(float sIntensity, float shine)
	: m_SpecularIntensity{ sIntensity }, m_Shininess{ shine }
{
}

Material::~Material()
{
}

void Material::UseMaterial(Shader & shader)
{
	shader.SetUniform1f("u_Material.specularIntensity", m_SpecularIntensity);
	shader.SetUniform1f("u_Material.shininess", m_Shininess);
}
