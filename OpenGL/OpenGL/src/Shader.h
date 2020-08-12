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

	void Bind() const; //함수는 glUseProgram()이지만, 앞서 설명한 것과 같이 바인딩("작업 상태로 만듬")과 같은 역할이기 때문에 Bind()로 통일
	void Unbind() const;

	//Set Uniforms
	void SetUniform1i(const std::string& name, int value); //shader slot을 명시하기 위한 int uniform 설정 함수 추가
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1f(const std::string& name, float value);
	void SetUniformMat4f(const std::string& name, const glm::mat4& matrix); //4x4 matrix를 전달하기 위한 함수 추가
private:
	ShaderProgramSource ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const std::string& vertexShader, const std::string& fragShader);
	
	int GetUniformLocation(const std::string& name);
};