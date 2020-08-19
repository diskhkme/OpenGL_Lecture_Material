#include "ShadowMap.h"
#include "Renderer.h"

#include <iostream>

ShadowMap::ShadowMap(int width, int height)
	:m_ShadowWidth{ width }, m_ShadowHeight{ height }
{
	//추가적인 프레임 버퍼 객체를 만듬
	//프레임 버퍼는 기본적으로 화면에 그려지는 Color, depth 버퍼 등이 포함된 버퍼로,
	//이미 생성이 되어 활용하고 있으나, Shadow처럼 화면을 한번 그린 후 그 데이터를 사용하고 싶을 때는
	//추가적인 화면에 그려지지 않는(모니터에 표시하지 않는) 프레임버퍼를 만들어 사용 가능함
	GLCall(glGenFramebuffers(1, &m_FBO));

	//프레임 버퍼에 그려진 픽셀 데이터는 텍스처로 활용한
	GLCall(glGenTextures(1, &m_ShadowMap));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ShadowMap));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER)); //바꾼 파라메터 효과 확인
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor)); //shadow map 생성 외의 영역은 그림자가 지지 않는 것이 더 현실적

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	//프레임 버퍼와 texture의 연결
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0)); 

	//우리가 추가한 프레임버퍼는 화면에 그리기 위한 용도가 아님을 알림
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));

	GLCall(int status = glCheckFramebufferStatus(GL_FRAMEBUFFER));

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "프레임 버퍼 오류 : " << status << std::endl;
		return;
	}

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

ShadowMap::~ShadowMap()
{
	if (m_FBO)
		GLCall(glDeleteFramebuffers(1, &m_FBO));
	
	if(m_ShadowMap)
		GLCall(glDeleteTextures(1, &m_FBO));
}

void ShadowMap::Bind()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
}

void ShadowMap::Unbind()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void ShadowMap::Read(unsigned int slot)
{
	//Texture 클래스의 Bind와 동일
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ShadowMap));
}
