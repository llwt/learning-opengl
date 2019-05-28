#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Debug.h"

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

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

	// enable transparency blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	float positions[] = {
		// "full screen"
		-1.0f, -1.0f, 0.0f, 0.0f, // 0 -- bottom left
		 1.0f, -1.0f, 1.0f, 0.0f, // 1 -- bottom right 
		 1.0f,  1.0f, 1.0f, 1.0f, // 2 -- top right
		-1.0f,  1.0f, 0.0f, 1.0f  // 3 -- top left
		
		/* // "centered
		-0.5f, -0.5f, 0.0f, 0.0f, // 0 -- bottom left
		 0.5f, -0.5f, 1.0f, 0.0f, // 1 -- bottom right 
		 0.5f,  0.5f, 1.0f, 1.0f, // 2 -- top right
		-0.5f,  0.5f, 0.0f, 1.0f  // 3 -- top left
		*/
	};

	VertexArray va;
	VertexBuffer vb(positions, sizeof(positions));
	VertexBufferLayout layout;
	layout.Push<float>(2); // vertex coordinates
	layout.Push<float>(2); // texture coordinates
	va.AddBuffer(vb, layout);

	glEnableVertexAttribArray(0);

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};
	IndexBuffer ib(indices, 6);

	Shader shader("Basic");
	shader.Bind();
	shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

	Texture texture("res/textures/tenor.png");
	//Texture texture("res/textures/dice.png");
	texture.Bind(0);
	shader.SetUniform1i("u_Texture", 0);

	// Unbind everything (so we can play around with rebinding before drawing and vaos)
	va.Unbind();
	vb.Unbind();
	ib.Unbind();
	shader.Unbind();

	Renderer renderer;

	float r = 0.00f;
	float increment = 0.05f;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear();

		/* Start rebinding stuff we explicity unbound */
		shader.Bind();
		shader.SetUniform4f("u_Color", r, 0.3f, 0.8f, 1.0f);

		renderer.Draw(va, ib, shader);


		/* End of rebinding */

		if (r > 1.0f)
			increment = -0.05f;
		else if (r < 0.0f)
			increment = 0.05f;

		r += increment;

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}