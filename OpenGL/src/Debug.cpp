#include "Debug.h"

#include <iostream>


void GLClearError() 
{
	while (glGetError() != GL_NO_ERROR);	
}

bool GLLogCall(const char* function, const char* file, int line)
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

/**
 * Borrowed from: https://github.com/SaschaWillems/openglcpp/blob/master/computeShader/computeShaderParticleSystem/main.cpp#L44
 */
void APIENTRY glDebugCallback(
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

