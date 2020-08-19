#include "ShadowMap.h"
#include "Renderer.h"

#include <iostream>

ShadowMap::ShadowMap(int width, int height)
	:m_ShadowWidth{ width }, m_ShadowHeight{ height }
{
	//�߰����� ������ ���� ��ü�� ����
	//������ ���۴� �⺻������ ȭ�鿡 �׷����� Color, depth ���� ���� ���Ե� ���۷�,
	//�̹� ������ �Ǿ� Ȱ���ϰ� ������, Shadowó�� ȭ���� �ѹ� �׸� �� �� �����͸� ����ϰ� ���� ����
	//�߰����� ȭ�鿡 �׷����� �ʴ�(����Ϳ� ǥ������ �ʴ�) �����ӹ��۸� ����� ��� ������
	GLCall(glGenFramebuffers(1, &m_FBO));

	//������ ���ۿ� �׷��� �ȼ� �����ʹ� �ؽ�ó�� Ȱ����
	GLCall(glGenTextures(1, &m_ShadowMap));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ShadowMap));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_ShadowWidth, m_ShadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER)); //�ٲ� �Ķ���� ȿ�� Ȯ��
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	float bColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLCall(glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bColor)); //shadow map ���� ���� ������ �׸��ڰ� ���� �ʴ� ���� �� ������

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	//������ ���ۿ� texture�� ����
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_ShadowMap, 0)); 

	//�츮�� �߰��� �����ӹ��۴� ȭ�鿡 �׸��� ���� �뵵�� �ƴ��� �˸�
	GLCall(glDrawBuffer(GL_NONE));
	GLCall(glReadBuffer(GL_NONE));

	GLCall(int status = glCheckFramebufferStatus(GL_FRAMEBUFFER));

	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "������ ���� ���� : " << status << std::endl;
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
	//Texture Ŭ������ Bind�� ����
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ShadowMap));
}
