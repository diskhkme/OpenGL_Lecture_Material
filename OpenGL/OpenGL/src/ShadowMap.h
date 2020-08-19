#pragma once

#include <GL/glew.h>

class ShadowMap
{
public:
	ShadowMap(int width, int height);
	~ShadowMap();

	virtual void Bind();
	virtual void Unbind();
	virtual void Read(unsigned int slot);

	int GetShadowWidth() { return m_ShadowWidth; }
	int GetShadowHeight() { return m_ShadowHeight; }
protected:
	unsigned int m_FBO;
	unsigned int m_ShadowMap;
	int m_ShadowWidth;
	int m_ShadowHeight;
};