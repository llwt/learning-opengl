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

#include "tests/TestClearColor.h"
#include "tests/TestMultipleViewports.h"

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

	Renderer renderer;

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
	
	struct TestCase {
		const char* label;
		test::Test* test;
	};

	TestCase* tests[] = {
		new TestCase{ "Multiple Viewports", new test::TestMultipleViewports() },
		new TestCase{ "Clear Color",        new test::TestClearColor() },
	};

	static const char* selectedLabel = NULL;
	TestCase *currentTest = NULL;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		renderer.Clear();

		if (currentTest) {
			currentTest->test->OnUpdate(0.0f);
			currentTest->test->OnRender(renderer, windowX, windowY);
		}

		// Start the ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

        ImGui::Begin("Test Selector");                     
		if (ImGui::BeginCombo("Test name", selectedLabel)) 
		{
			for (int i = 0; i < IM_ARRAYSIZE(tests); i++) 
			{
				bool isSelected = selectedLabel == tests[i]->label;
				if (ImGui::Selectable(tests[i]->label, isSelected)) 
				{
					currentTest = tests[i];
					selectedLabel = currentTest->label;
				};
			}
			ImGui::EndCombo();
		}
		ImGui::End();

		if (currentTest) {
			currentTest->test->OnImGuiRender(windowX, windowY);
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