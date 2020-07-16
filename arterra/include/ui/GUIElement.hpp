#pragma once

#include "PCH.hpp"

#include "renderer/VertexArray.hpp"

namespace arterra {
	
	class GuiElement {
		
		public:
			GuiElement();
			GuiElement(std::vector<float_t> posVertices, std::vector<float_t> uvVertices);
			GuiElement(GuiElement& guiElement);
			
			void Destroy();
			void GenerateMesh();
			
			inline void SetShouldRender(bool shouldRender) { _shouldRender = shouldRender; }
			inline bool ShouldRender() const { return _shouldRender; }
			
			inline void Bind() { _vertexArray.Bind(); }
			inline GLuint GetVertexCount() { return _vertexArray.GetVertexCount(); }
			
		private:
			std::vector<float_t> _posVertices;
			std::vector<float_t> _uvVertices;
			VertexArray _vertexArray;

			VertexBuffer _posBuffer;
			VertexBuffer _texBuffer;
			
			bool _shouldRender = false;
	};
	
}