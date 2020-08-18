#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>

#include "Renderer.h"

#include "glm/glm.hpp"

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
	void SetUniform3f(const std::string& name, float v0, float v1, float v2);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1f(const std::string& name, float value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix); //4x4 matrix�� �����ϱ� ���� �Լ� �߰�
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader);
	
	int GetUniformLocation(const std::string& name);
};