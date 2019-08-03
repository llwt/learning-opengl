#include "TestClearColor.h"

#include "Renderer.h"
#include "imgui/imgui.h"


namespace test {
	TestClearColor::TestClearColor()
		: m_ClearColor { 0.2f, 0.4f, 0.8f, 1.0f}
	{
	}


	TestClearColor::~TestClearColor()
	{
	}

	void TestClearColor::OnUpdate(float deltaTime)
	{

	}

	void TestClearColor::OnRender(Renderer &renderer, unsigned int windowX, unsigned int windowY)
	{
		glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	void TestClearColor::OnImGuiRender(unsigned int windowX, unsigned int windowY)
	{
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
	}
}

