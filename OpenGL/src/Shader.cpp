#include <iostream>
#include <fstream>
#include <streambuf>

#include "Shader.h"
#include "Renderer.h"

Shader::Shader(const std::string & name)
	: m_ShaderName(name), m_RendererID(0)
{
	// TODO: work in constructor feels dirty
	ShaderProgramSource source = ReadShaderSource();
	m_RendererID = CreateShader(source);
}


Shader::~Shader()
{
	glDeleteProgram(m_RendererID);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererID);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string & name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform4f(const std::string & name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}


std::string Shader::ReadShaderFile(const std::string& shaderFile)
{
	std::ifstream in("res/shaders/" + shaderFile);
	if (in)
	{
		return (std::string(
			(std::istreambuf_iterator<char>(in)), 
			std::istreambuf_iterator<char>())
		);
	}
	throw(errno);
}

ShaderProgramSource Shader::ReadShaderSource()
{
	std::string vShader = ReadShaderFile(m_ShaderName + ".vert");
	std::string fShader = ReadShaderFile(m_ShaderName + ".frag");

	return { vShader, fShader };
}

unsigned int Shader::HandleCompileShaderError(unsigned int id, unsigned int type) {
	int length;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

	// TODO: did "theCherno" make a video about alloca
	/**
	 *NOTE: this is the same as `char errorMessage[length]` but c++ 
	 *	    doesn't like dynamic values in for stack allocations 
	 *      (as opposed to heap allocations, e.g. char* foo = new char[length];)
	 */
	char* errorMessage = (char*)alloca(length * sizeof(char));
	glGetShaderInfoLog(id, length, &length, errorMessage);

	std::cout << "Failed to compile " 
			  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			  << "shader!" << std::endl;
	std::cout << errorMessage << std::endl;

	glDeleteShader(id);

	return 0;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Handle compile errors (syntax, et.c)
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE)
	{
		return HandleCompileShaderError(id, type);
	}

	return id;
}

unsigned int Shader::CreateShader(const ShaderProgramSource& source)
{
	unsigned int program = glCreateProgram();
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, source.Vertex);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, source.Fragment);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	// TODO: read up on these
	glLinkProgram(program);
	glValidateProgram(program);

	// Delete intermediate shaders now that they have been linked into the program
	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

int Shader::GetUniformLocation(const std::string& name)
{
	if (m_UniformLocationCache.find(name) == m_UniformLocationCache.end()) {
		m_UniformLocationCache[name] = glGetUniformLocation(m_RendererID, name.c_str());

		if (m_UniformLocationCache[name] == -1)
		{
			std::cout << "Warning: uniform '" << name << "'"
					  << ", requested in shader '" << m_ShaderName << "',"
					  << " dosn't exist!";
		}
	}

	return m_UniformLocationCache[name];
}

