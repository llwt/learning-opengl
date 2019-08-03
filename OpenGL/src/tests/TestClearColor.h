#pragma once

#include "Test.h"

namespace test {
	class TestClearColor : public Test
	{
	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender(Renderer &renderer, unsigned int windowX, unsigned int windowY) override;
		void OnImGuiRender(unsigned int windowX, unsigned int windowY) override;
	private:
		float m_ClearColor[4];
	};
}

