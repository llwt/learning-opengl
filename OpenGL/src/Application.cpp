#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <streambuf>

#define ASSERT(x) if (!(x)) __debugbreak();
#define GLCall(x) GLClearError();\
	x;\
	ASSERT(GLLogCall(#x, __FILE__, __LINE__))

static void GLClearError() 
{
	while (glGetError() != GL_NO_ERROR);	
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] " 
			      << "(" << error << ") " 
			      << function
				  << " " << file << ":" << line
			      << std::endl;
		return false;
	}

	return true;
}

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
	GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));

	// TODO: did "theCherno" make a video about alloca
	/**
	 *NOTE: this is the same as `char errorMessage[length]` but c++ 
	 *	    doesn't like dynamic values in for stack allocations 
	 *      (as opposed to heap allocations, e.g. char* foo = new char[length];)
	 */
	char* errorMessage = (char*)alloca(length * sizeof(char));
	GLCall(glGetShaderInfoLog(id, length, &length, errorMessage));

	std::cout << "Failed to compile " 
			  << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") 
			  << "shader!" << std::endl;
	std::cout << errorMessage << std::endl;

	GLCall(glDeleteShader(id));

	return 0;
}

static unsigned int CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	// Handle compile errors (syntax, et.c)
	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE)
	{
		return HandleCompileShaderError(id, type);
	}

	return id;
}

static unsigned int CreateShader(const ShaderProgramSource& source)
{
	GLCall(unsigned int program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, source.Vertex);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, source.Fragment);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	// TODO: read up on these
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	// Delete intermediate shaders now that they have been linked into the program
	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	GLenum glewErr = glewInit();
	if (GLEW_OK != glewErr)
	{
	  fprintf(stderr, "GLEW init error: %s\n", glewGetErrorString(glewErr));
	  return -1;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
	
	float positions[] = {
		-0.5f, -0.5f,
		 0.5f, -0.5f,
		 0.5f,  0.5f,
		-0.5f,  0.5f,
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	unsigned int buffer;
	GLCall(glGenBuffers(1, &buffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, buffer));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW));

	const unsigned int POSITION_ATTRIB = 0;
	GLCall(glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0));
	GLCall(glEnableVertexAttribArray(POSITION_ATTRIB));

	unsigned int ibo;
	GLCall(glGenBuffers(1, &ibo));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW));

	ShaderProgramSource source = ReadShaderSource("Basic");
	unsigned int shader = CreateShader(source);
	GLCall(glUseProgram(shader));

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr));

		/* Swap front and back buffers */
		GLCall(glfwSwapBuffers(window));

		/* Poll for and process events */
		glfwPollEvents();
	}

	GLCall(glDeleteProgram(shader));

	glfwTerminate();
	return 0;
}