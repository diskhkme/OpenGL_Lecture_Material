#include "Texture.h"
#include "stb_image/stb_image.h" //src/vendor�� ������Ʈ �Ӽ����� �߰���

#include <iostream>

Texture::Texture(const std::string & path)
	:m_RendererID{ 0 }, m_FilePath {path}, m_LocalBuffer{ nullptr }, m_Width{ 0 }, m_Height{ 0 }, m_BPP{ 0 }
{
	stbi_set_flip_vertically_on_load(1); //�ؽ�ó ���Ʒ��� ������(OpenGL�� ��ǥ�� �ٸ�)
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4); //��ũ���� �̹��� �о����
	if (!m_LocalBuffer)
	{
		std::cout << "������ �����ϴ� : " << path << std::endl;
		return;
	}

	GLCall(glGenTextures(1, &m_RendererID)); //�ؽ�ó ���� ����
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); //���ε�

	//�ؽ�ó ó�� �Ķ���� (Chapter 8���� ���� �ٷ�)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	//�ؽ�ó ������ ����
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); //������ �������� ����ε�

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer); //���� free
	}
		
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); //�ؽ�ó�� Ȱ��ȭ �� �� �ִ� ������ �����Ǿ� ������, �̸� ����ؾ� ��
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

}
