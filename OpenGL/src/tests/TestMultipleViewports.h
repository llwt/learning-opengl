#pragma once

#include "Test.h"
#include "Renderer.h"
#include "Texture.h"

namespace test {
	class TestMultipleViewports : public Test
	{
	public:
		TestMultipleViewports();
		~TestMultipleViewports();

		void OnUpdate(float deltaTime) override;
		void OnRender(Renderer &renderer, unsigned int windowX, unsigned int windowY) override;
		void OnImGuiRender(unsigned int windowX, unsigned int windowY) override;
	private:
		float m_Scale;
		float m_Increment;

		glm::vec3 m_ModelTranslationA;
		glm::vec3 m_ModelTranslationB;
		glm::mat4 m_View;
		glm::vec4 m_Color;

		float* m_Positions;
		VertexArray m_VertexArray;
		VertexBuffer m_VertexBuffer;
		VertexBufferLayout m_Layout;

		unsigned int* m_Indicies;
		IndexBuffer m_IndexBuffer;

		Shader m_Shader;
		Texture m_Texture;

	};
}
