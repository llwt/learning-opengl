#include <algorithm>

#include "TestMultipleViewports.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"

namespace test {
	TestMultipleViewports::TestMultipleViewports()
		: m_Scale(2.0f),
		m_Increment(0.05f),
		m_ModelTranslationA(0, 0, 0),
		m_ModelTranslationB(200, 200, 0),
		m_View(glm::translate(
			glm::mat4(1.0f),	  // identity TODO: why is this needed?
			//glm::vec3(-100, 0, 0) // Moving camera "right" means shifting verticies "left"
			glm::vec3(0, 0, 0)    // Noop translation
		)),
		m_Color(0.00f, 0.3f, 0.8f, 1.0f),
		m_Positions(new float[16]{
			// "bottom left square" 960/540 
			-50.0f * m_Scale, -50.0f * m_Scale, 0.0f, 0.0f, // 0 -- bottom left
			-50.0f * m_Scale,  50.0f * m_Scale, 1.0f, 0.0f, // 1 -- bottom right 
			 50.0f * m_Scale,  50.0f * m_Scale, 1.0f, 1.0f, // 2 -- top right
			 50.0f * m_Scale, -50.0f * m_Scale, 0.0f, 1.0f  // 3 -- top left

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
		}),
		m_VertexBuffer(m_Positions, 16 * sizeof(float)),
		m_Indicies(new unsigned int[6]{
			0, 1, 2,
			2, 3, 0
		}),
		m_IndexBuffer(m_Indicies, 6),
		m_Shader("Basic"),
		//Texture texture("res/textures/dice.png");
		m_Texture("res/textures/tenor.png")
	{
		// SHould this be in the constructor or in something like an "onLoad" method?
		m_Layout.Push<float>(2); // vertex coordinates
		m_Layout.Push<float>(2); // texture coordinates
		m_VertexArray.AddBuffer(m_VertexBuffer, m_Layout);

		m_Shader.Bind();
		m_Shader.SetUniform4f("u_Color", 0.8f, 0.3f, 0.8f, 1.0f);

		m_Texture.Bind(0);
		m_Shader.SetUniform1i("u_Texture", 0);

		// Unbind everything (so we can play around with rebinding before drawing and vaos)
		m_VertexArray.Unbind();
		m_VertexBuffer.Unbind();
		m_IndexBuffer.Unbind();
		m_Shader.Unbind();
	}


	TestMultipleViewports::~TestMultipleViewports()
	{
	}

	void test::TestMultipleViewports::OnUpdate(float deltaTime)
	{
		if (m_Color.r > 1.0f)
			m_Increment = -0.05f;
		else if (m_Color.r < 0.0f)
			m_Increment = 0.05f;

		m_Color.r += m_Increment;
	}

	void test::TestMultipleViewports::OnRender(Renderer &renderer, unsigned int windowX, unsigned int windowY)
	{
		/* Start rebinding stuff we explicity unbound */
		m_Shader.Bind();
		m_Shader.SetUniform4f("u_Color", m_Color.r, m_Color.g, m_Color.b, m_Color.a);

		// map projection to pixel space
		glm::mat4 proj = glm::ortho(0.0f, (float)windowX, 0.0f, (float)windowY, -1.0f, 1.0f);

		/* Draw instance with "A" translation */
		{
			glm::mat4 model = glm::translate( // Move object "up" and to the "right" 200px
				glm::mat4(1.0f),
				m_ModelTranslationA
			); 
			glm::mat4 mvp = proj * m_View * model;  // right to left (PVM) due to matrix structure in OpenGL
			m_Shader.SetUniformMatrix4f("u_MVP", mvp);

			renderer.Draw(m_VertexArray, m_IndexBuffer, m_Shader);
		}

		/* Draw instance againt but with "B" translation */
		{
			glm::mat4 model = glm::translate( // Move object "up" and to the "right" 200px
				glm::mat4(1.0f),
				m_ModelTranslationB
			); 
			glm::mat4 mvp = proj * m_View * model;  // right to left (PVM) due to matrix structure in OpenGL
			m_Shader.SetUniformMatrix4f("u_MVP", mvp);

			renderer.Draw(m_VertexArray, m_IndexBuffer, m_Shader);
		}
	}

	void test::TestMultipleViewports::OnImGuiRender(unsigned int windowX, unsigned int windowY)
	{
        ImGui::Begin("Debug");                     
		ImGui::SliderFloat2("A: X & Y", &m_ModelTranslationA.x, 0.0f, std::max((float)windowX, (float)windowY));
		ImGui::SliderFloat2("B: X & Y", &m_ModelTranslationB.x, 0.0f, std::max((float)windowX, (float)windowY));
		ImGui::ColorEdit4("color", (float*)&m_Color.r);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}
