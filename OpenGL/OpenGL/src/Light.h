#pragma once

#include "glm/glm.hpp"
#include "Shader.h"

//다양한 종류의 Light가 있을 수 있으므로, light는 Base class로 하고,
//특정 light를 유도 클래스로 생성하도록 함.

#include "ShadowMap.h" //라이트는 shadow map을 그릴 수 있음

class Light
{
public:
	Light();
	Light(int shadowWidth, int shadowHeight,
		glm::vec3 aColor, float aIntensity, float dIntensity); //모든 light가 방향을 갖는 것은 아님
	virtual ~Light();

	ShadowMap* GetShadowMap() { return m_ShadowMap; }
	//입력해야 하는 uniform도 light 종류에 따라 다름
protected:
	glm::vec3 m_LightColor; //s_a & s_d & s_s의 색상

	float m_AmbientIntensity; //Ambient 세기 조절 (s_a = m_LightColor * m_AmbientIntensity)
	float m_DiffuseIntensity; //Diffuse 세기 조절 (s_a = m_LightColor * m_AmbientIntensity)

	glm::mat4 m_LightProjection; //shadow map 계산을 위한 변환 행렬
	ShadowMap* m_ShadowMap; //shadow map 객체
};