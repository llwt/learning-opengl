#pragma once

#include <string>
#include <unordered_map>

struct ShaderProgramSource
{
	std::string Vertex;
	std::string Fragment;
};

class Shader
{
private:
	std::string m_ShaderName;
	unsigned int m_RendererID;
	std::unordered_map<std::string, int> m_UniformLocationCache;
public:
	Shader(const std::string& shaderName);
	~Shader();

	void Bind() const;
	void Unbind() const;

	// Set uniforms
	void SetUniform1i(const std::string& name, int value);
	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
private:
	std::string ReadShaderFile(const std::string& shaderFile);
	ShaderProgramSource ReadShaderSource();
	unsigned int HandleCompileShaderError(unsigned int id, unsigned int type);
	unsigned int CompileShader(unsigned int type, const std::string& source);
	unsigned int CreateShader(const ShaderProgramSource& source);
	int GetUniformLocation(const std::string& name);
};

