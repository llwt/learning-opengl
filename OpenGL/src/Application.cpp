#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <streambuf>

static void GLClearError() 
{
	while (glGetError() != GL_NO_ERROR);	
}

static bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError())
	{
		std::cout << "[OpenGL Error] " 
			      << "(" << error << ")" 
			      << function
				  << " " << file
				  << ":" << line
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

/**
 * Borrowed from: https://github.com/SaschaWillems/openglcpp/blob/master/computeShader/computeShaderParticleSystem/main.cpp#L44
 */
static void APIENTRY glDebugCallback(
	GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar* message,
	const GLvoid* userParam
) {
	std::string msgSource;
	switch (source){
	case GL_DEBUG_SOURCE_API:
		msgSource = "WINDOW_SYSTEM";
		break;
	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		msgSource = "SHADER_COMPILER";
		break;
	case GL_DEBUG_SOURCE_THIRD_PARTY:
		msgSource = "THIRD_PARTY";
		break;
	case GL_DEBUG_SOURCE_APPLICATION:
		msgSource = "APPLICATION";
		break;
	case GL_DEBUG_SOURCE_OTHER:
		msgSource = "OTHER";
		break;
	}

	std::string msgType;
	switch (type) {
		case GL_DEBUG_TYPE_ERROR:
			msgType = "ERROR";
			break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
			msgType = "DEPRECATED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
			msgType = "UNDEFINED_BEHAVIOR";
			break;
		case GL_DEBUG_TYPE_PORTABILITY:
			msgType = "PORTABILITY";
			break;
		case GL_DEBUG_TYPE_PERFORMANCE:
			msgType = "PERFORMANCE";
			break;
		case GL_DEBUG_TYPE_OTHER:
			msgType = "OTHER";
			break;
	}

	std::string msgSeverity;
	switch (severity){
		case GL_DEBUG_SEVERITY_NOTIFICATION:
			msgSeverity = "Notification";
			break;
		case GL_DEBUG_SEVERITY_LOW:
			msgSeverity = "LOW";
			break;
		case GL_DEBUG_SEVERITY_MEDIUM:
			msgSeverity = "MEDIUM";
			break;
		case GL_DEBUG_SEVERITY_HIGH:
			msgSeverity = "HIGH";
			break;
	}

	printf("glDebugMessage:\n%s \n type = %s source = %s severity = %s\n", message, msgType.c_str(), msgSource.c_str(), msgSeverity.c_str());
	if (severity != GL_DEBUG_SEVERITY_NOTIFICATION) 
	{
		__debugbreak();
	}
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

	// Set debug callback
	if (glDebugMessageCallback != NULL) {
		glDebugMessageCallback(glDebugCallback, NULL);
	}
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS); // so the debugbreak has the call stack
	glEnable(GL_DEBUG_OUTPUT);
	
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
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), &positions, GL_STATIC_DRAW);

	const unsigned int POSITION_ATTRIB = 0;
	glVertexAttribPointer(POSITION_ATTRIB, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
	glEnableVertexAttribArray(POSITION_ATTRIB);

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), &indices, GL_STATIC_DRAW);

	ShaderProgramSource source = ReadShaderSource("Basic");
	unsigned int shader = CreateShader(source);
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glDeleteProgram(shader);

	glfwTerminate();
	return 0;
}