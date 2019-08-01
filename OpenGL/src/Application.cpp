#include <iostream>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

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
	const char* glsl_version = "#version 130"; // for imgui (TODO: why does this blow up if set to 133?)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE); // <<-- Default
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	/* Create a windowed mode window and its OpenGL context */
	//window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
	unsigned int windowX = 960;
	unsigned int windowY = 540;

	window = glfwCreateWindow(windowX, windowY, "Hello World", NULL, NULL);
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
	
	float scale = 2.0f;
	
	float positions[] = {
		// "bottom left square" 960/540 
		-50.0f * scale, -50.0f * scale, 0.0f, 0.0f, // 0 -- bottom left
		-50.0f * scale,  50.0f * scale, 1.0f, 0.0f, // 1 -- bottom right 
		 50.0f * scale,  50.0f * scale, 1.0f, 1.0f, // 2 -- top right
		 50.0f * scale, -50.0f * scale, 0.0f, 1.0f  // 3 -- top left

		// "full screen" 1:1
		// -1.0f, -1.0f, 0.0f, 0.0f, // 0 -- bottom left
		//  1.0f, -1.0f, 1.0f, 0.0f, // 1 -- bottom right 
		//  1.0f,  1.0f, 1.0f, 1.0f, // 2 -- top right
		// -1.0f,  1.0f, 0.0f, 1.0f  // 3 -- top left
		
		/* // "centered 1:1
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

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

	float increment = 0.05f;
	// glm::vec3 modelTranslation = glm::vec3(200, 200, 0);
	glm::vec3 modelTranslationA = glm::vec3(0, 0, 0);
	glm::vec3 modelTranslationB = glm::vec3(200, 200, 0);
	glm::vec4 color = glm::vec4(0.00f, 0.3f, 0.8f, 1.0f);


	/*
	glm::mat4 view = glm::translate( // Move camera 100px to the "right"
		glm::mat4(1.0f),			   // identity TODO: why is this needed?
		glm::vec3(-100, 0, 0)          // Moving camera "right" means shifting verticies "left"
	);
	*/
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)); // Noop translation

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear();

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		/* Start rebinding stuff we explicity unbound */
		shader.Bind();
		shader.SetUniform4f("u_Color", color.r, color.g, color.b, color.a);

		// map projection to pixel space
		glm::mat4 proj = glm::ortho(0.0f, (float)windowX, 0.0f, (float)windowY, -1.0f, 1.0f);

		/* Draw instance with "A" translation */
		{
			glm::mat4 model = glm::translate( // Move object "up" and to the "right" 200px
				glm::mat4(1.0f),
				modelTranslationA
			); 
			glm::mat4 mvp = proj * view * model;  // right to left (PVM) due to matrix structure in OpenGL
			shader.SetUniformMatrix4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);
		}

		/* Draw instance againt but with "B" translation */
		{
			glm::mat4 model = glm::translate( // Move object "up" and to the "right" 200px
				glm::mat4(1.0f),
				modelTranslationB
			); 
			glm::mat4 mvp = proj * view * model;  // right to left (PVM) due to matrix structure in OpenGL
			shader.SetUniformMatrix4f("u_MVP", mvp);

			renderer.Draw(va, ib, shader);
		}


		/* End of rebinding */

		if (color.r > 1.0f)
			increment = -0.05f;
		else if (color.r < 0.0f)
			increment = 0.05f;

		color.r += increment;

        {
            ImGui::Begin("Debug");                     
			ImGui::SliderFloat2("A: X & Y", &modelTranslationA.x, 0.0f, std::max((float)windowX, (float)windowY));
			ImGui::SliderFloat2("B: X & Y", &modelTranslationB.x, 0.0f, std::max((float)windowX, (float)windowY));
            ImGui::ColorEdit4("color", (float*)&color.r);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }


        ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	// ImgGui Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}