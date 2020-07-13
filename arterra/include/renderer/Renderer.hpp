#pragma once
#include "PCH.hpp"

#include <glad/glad.h>

namespace arterra {
	class Renderer {
	public:
		Renderer();

		// Update the renderer to account for input
		void Update();

		// Draw a triangle array
		void DrawTriangles(size_t vertexCount);

	private:
	};

}