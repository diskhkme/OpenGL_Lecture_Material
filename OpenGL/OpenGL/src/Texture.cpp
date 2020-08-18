#include "Texture.h"
#include "stb_image/stb_image.h" //src/vendor를 프로젝트 속성에서 추가함

#include <iostream>

Texture::Texture(const std::string & path)
	:m_RendererID{ 0 }, m_FilePath {path}, m_LocalBuffer{ nullptr }, m_Width{ 0 }, m_Height{ 0 }, m_BPP{ 0 }
{
	stbi_set_flip_vertically_on_load(1); //텍스처 위아래를 뒤집음(OpenGL은 좌표계 다름)
	m_LocalBuffer = stbi_load(path.c_str(), &m_Width, &m_Height, &m_BPP, 4); //디스크에서 이미지 읽어오기
	if (!m_LocalBuffer)
	{
		std::cout << "파일이 없습니다 : " << path << std::endl;
		return;
	}

	GLCall(glGenTextures(1, &m_RendererID)); //텍스처 버퍼 생성
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID)); //바인딩

	//텍스처 처리 파라메터 (Chapter 8에서 상세히 다룸)
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

	//텍스처 데이터 전달
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0)); //전달이 끝났으니 언바인딩

	if (m_LocalBuffer)
	{
		stbi_image_free(m_LocalBuffer); //버퍼 free
	}
		
}

Texture::~Texture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void Texture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot)); //텍스처는 활성화 할 수 있는 슬롯이 한정되어 있으며, 이를 명시해야 함
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void Texture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

}
