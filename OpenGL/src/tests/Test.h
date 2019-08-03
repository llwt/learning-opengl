#pragma once

#include "Renderer.h"

namespace test {
	class Test
	{
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		// TODO: move windowX and windowY to constructor
		virtual void OnRender(Renderer &renderer, unsigned int windowX, unsigned int windowY) {}
		virtual void OnImGuiRender(unsigned int windowX, unsigned int windowY) {}
	};
}
