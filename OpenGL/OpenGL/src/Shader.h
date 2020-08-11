#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "Renderer.h"

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragSource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& filepath);
	~Shader();

	void Bind() const; //�Լ��� glUseProgram()������, �ռ� ������ �Ͱ� ���� ���ε�("�۾� ���·� ����")�� ���� �����̱� ������ Bind()�� ����
	void Unbind() const;

	//Set Uniforms
	void SetUniform1i(const std::string& name, int value); //shader slot�� ����ϱ� ���� int uniform ���� �Լ� �߰�
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1f(const std::string& name, float value);
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader);
	
	int GetUniformLocation(const std::string& name);
};