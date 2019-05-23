#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <streambuf>

#include "Debug.h"

#include "IndexBuffer.h"
#include "VertexBuffer.h"

static std::string ReadShaderFile(const std::string& shaderFile)
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

struct ShaderProgramSource
{
	std::string Vertex;
	std::string Fragment;
};

static ShaderProgramSource ReadShaderSource(const std::string& shaderName)
{
	std::string vShader = ReadShaderFile(shaderName + ".vert");
	std::string fShader = ReadShaderFile(shaderName + ".frag");
	
	return { vShader, fShader };
}

static unsigned int HandleCompileShaderError(unsigned int id, unsigned int type) {
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

static unsigned int CompileShader(unsigned int type, const std::string& source)
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

static unsigned int CreateShader(const ShaderProgramSource& source)
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

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Configure specific glfw/opengl params */
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // <<-- Default
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1);

	GLenum glewErr = glewInit();
	if (GLEW_OK != glewErr)
	{
	  fprintf(stderr, "GLEW init error: %s\n", glewGetErrorString(glewErr));
	  return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	// Set debug callback
	if (glDebugMessageCallback != NULL) {
		glDebugMessageCallback(glDebugCallback, NULL);
	}
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // so the debugbreak has the call stack
	glEnable(GL_DEBUG_OUTPUT);
	


	unsigned int vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	float positions[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f,
	};
	VertexBuffer vb(positions, sizeof(positions));

	const unsigned int POSITION_ATTRIB_IDX = 0;
	glVertexAttribPointer(POSITION_ATTRIB_IDX, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(POSITION_ATTRIB_IDX);

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	IndexBuffer ib(indices, 6);

	ShaderProgramSource source = ReadShaderSource("Basic");
	unsigned int shader = CreateShader(source);
	glUseProgram(shader);

	int location = glGetUniformLocation(shader, "u_Color");
	ASSERT(location != -1);

	// Unbind everything (so we can play around with rebinding before drawing and vaos)
	glBindVertexArray(0);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	float r = 0.00f;
	float increment = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		/* Start rebinding stuff we explicity unbound */
		glUseProgram(shader);
		glUniform4f(location, r, 0.3, 0.8, 1.0);

		glBindVertexArray(vao);
		ib.Bind();
		/* End of rebinding */

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05;

		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}