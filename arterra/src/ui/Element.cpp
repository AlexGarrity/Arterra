#include "ui/Element.hpp"

namespace arterra {
	
	namespace UI {
		
		ElementMesh::ElementMesh()
			: _posVertices(), _uvVertices(), _vertexArray(), _posBuffer(), _texBuffer() {}
		
		ElementMesh::ElementMesh(std::vector<float_t> posVertices, std::vector<float_t> uvVertices)
			: _posVertices(posVertices), _uvVertices(uvVertices) {
			GenerateMesh();
		}
		
		
		
		
		Element::Element() 
			: _width(0), _height(0), _position(glm::vec2(0.0f, 0.0f)), _rotation(0.0f),
			_positionAnchor(Anchor::BottomLeft),	_mesh(ElementMesh()), _material(Material()),
			_collider(ElementCollider()) {}
		
		Element::Element(int width, int height, glm::vec2 position, float_t rotation, Anchor anchor,
			AtlasTexture* texture, Material material)
			: _width(width), _height(height), _position(position), _rotation(glm::radians(rotation)), _positionAnchor(anchor),
			_material(material), _texture(texture) {
			
			_transform = glm::translate(_transform, { position.x, position.y, 0.0f });
			_transform = glm::scale(_transform, { width , height, 1.0f });
			_transform = glm::rotate(_transform, 0.0f, { 0.0f, 0.0f, 1.0f });
			_material.AddParameter(ShaderParameter { "u_ModelProjection", _transform, ShaderParameter::Type::Mat4});
			_material.AddParameter(ShaderParameter { "u_Rotation", _rotation, ShaderParameter::Type::Float});
		}
				
		void Element::ApplyTranslation(glm::vec2 movementVector) {
			_position.x += movementVector.x;
			_position.y += movementVector.y;
		}
		
		void Element::ApplyScaling(float_t scaleFactor) {
			_width += scaleFactor;
			_height += scaleFactor;
		}
		
		void Element::ApplyRotation(float_t rotationAngle) {
			// TODO: if greater than 360 loop round to 0.
			rotationAngle = glm::radians(rotationAngle);
			_rotation += rotationAngle;
		}
		
		void Element::UpdateTransform() {
			// WHEN TRANSFORMING!!!
			// Translate -> Rotate -> Scale ALWAYS!!!
			// Also, translate is not commutative,
			//		i.e, you can't add movement vector to existing position.
			_transform = glm::mat4 { 1.0f };
			_transform = glm::translate(_transform, { _position.x, _position.y, 0.0f });
			_transform = glm::rotate(_transform, _rotation, { 0.0f, 0.0f, -1.0f });
			_transform = glm::scale(_transform, { _width , _height, 1.0f });
			_material.UpdateParameter("u_ModelProjection", _transform);
			_collider.GenerateCollider();
		}
		
		void Element::CreateCollider() {
			_collider = ElementCollider { this };
		}
		
		void Element::CreateMesh() {
			// Reserve space for vertex position data.
			std::vector<float_t> positions;
			positions.resize(12);
			// Translate uv 0-1 coordinates to atlas coordinates.
			std::vector<float_t> uvs = {
				_texture->_x, _texture->_y,
				_texture->_x + _texture->_width, _texture->_y,
				_texture->_x, _texture->_y + _texture->_height,
				_texture->_x, _texture->_y + _texture->_height,
				_texture->_x + _texture->_width, _texture->_y,
				_texture->_x + _texture->_width, _texture->_y + _texture->_height
			};
			
			int halfwidth = 1 / 2;
			int halfheight = 1 / 2;
			
			// Calculate the vertex positions based off the anchor point.
			switch (_positionAnchor)
			{
			case Anchor::BottomLeft:
				positions = {
					0.0f, 0.0f,
					0.0f + 1.0f, 0.0f,
					0.0f, 0.0f + 1.0f,
					0.0f, 0.0f + 1.0f,
					0.0f + 1.0f, 0.0f,
					0.0f + 1.0f, 0.0f + 1.0f
				};
				break;
			case Anchor::BottomRight:
				positions = {
					0.0f - 1.0f, 0.0f,
					0.0f, 0.0f,
					0.0f - 1.0f, 0.0f + 1.0f,
					0.0f - 1.0f, 0.0f + 1.0f,
					0.0f, 0.0f,
					0.0f, 0.0f + 1.0f
				};
				break;
			case Anchor::BottomCentre:
				positions = {
					0.0f - halfwidth, 0.0f,
					0.0f + halfwidth, 0.0f,
					0.0f - halfwidth, 0.0f + 1.0f,
					0.0f - halfwidth, 0.0f + 1.0f,
					0.0f + halfwidth, 0.0f,
					0.0f + halfwidth, 0.0f + 1.0f
				};
				break;
			case Anchor::TopLeft:
				positions = {
					0.0f, 0.0f - 1.0f,
					0.0f + 1.0f, 0.0f - 1.0f,
					0.0f, 0.0f,
					0.0f, 0.0f,
					0.0f + 1.0f, 0.0f - 1.0f,
					0.0f + 1.0f, 0.0f
				};
				break;
			case Anchor::TopRight:
				positions = {
					0.0f - 1.0f, 0.0f - 1.0f,
					0.0f, 0.0f - 1.0f,
					0.0f - 1.0f, 0.0f,
					0.0f - 1.0f, 0.0f,
					0.0f, 0.0f - 1.0f,
					0.0f, 0.0f
				};
				break;
			case Anchor::TopCentre:
				positions = {
					0.0f - halfwidth, 0.0f - 1.0f,
					0.0f + halfwidth, 0.0f - 1.0f,
					0.0f - halfwidth, 0.0f,
					0.0f - halfwidth, 0.0f,
					0.0f + halfwidth, 0.0f - 1.0f,
					0.0f + halfwidth, 0.0f
				};
				break;
			case Anchor::Left:
				positions = {
					0.0f, 0.0f - halfheight,
					0.0f + 1.0f, 0.0f - halfheight,
					0.0f, 0.0f + halfheight,
					0.0f, 0.0f + halfheight,
					0.0f + 1.0f, 0.0f - halfheight,
					0.0f + 1.0f, 0.0f + halfheight
				};
				break;
			case Anchor::Right:
				positions = {
					0.0f - 1.0f, 0.0f - halfheight,
					0.0f, 0.0f - halfheight,
					0.0f - 1.0f, 0.0f + halfheight,
					0.0f - 1.0f, 0.0f + halfheight,
					0.0f, 0.0f - halfheight,
					0.0f, 0.0f + halfheight
				};
				break;
			case Anchor::Centre:
				positions = {
					0.0f - halfwidth, 0.0f - halfheight,
					0.0f + halfwidth, 0.0f - halfheight,
					0.0f - halfwidth, 0.0f + halfheight,
					0.0f - halfwidth, 0.0f + halfheight,
					0.0f + halfwidth, 0.0f - halfheight,
					0.0f + halfwidth, 0.0f + halfheight
				};
				break;
			}
			/* switch (_anchor)
			{
			case ElementAnchor::BottomLeft:
				positions = {
					_position.x, _position.y,
					_position.x + _width, _position.y,
					_position.x, _position.y + _height,
					_position.x, _position.y + _height,
					_position.x + _width, _position.y,
					_position.x + _width, _position.y + _height
				};
				break;
			case ElementAnchor::BottomRight:
				positions = {
					_position.x - _width, _position.y,
					_position.x, _position.y,
					_position.x - _width, _position.y + _height,
					_position.x - _width, _position.y + _height,
					_position.x, _position.y,
					_position.x, _position.y + _height
				};
				break;
			case ElementAnchor::BottomCentre:
				positions = {
					_position.x - halfwidth, _position.y,
					_position.x + halfwidth, _position.y,
					_position.x - halfwidth, _position.y + _height,
					_position.x - halfwidth, _position.y + _height,
					_position.x + halfwidth, _position.y,
					_position.x + halfwidth, _position.y + _height
				};
				break;
			case ElementAnchor::TopLeft:
				positions = {
					_position.x, _position.y - _height,
					_position.x + _width, _position.y - _height,
					_position.x, _position.y,
					_position.x, _position.y,
					_position.x + _width, _position.y - _height,
					_position.x + _width, _position.y
				};
				break;
			case ElementAnchor::TopRight:
				positions = {
					_position.x - _width, _position.y - _height,
					_position.x, _position.y - _height,
					_position.x - _width, _position.y,
					_position.x - _width, _position.y,
					_position.x, _position.y - _height,
					_position.x, _position.y
				};
				break;
			case ElementAnchor::TopCentre:
				positions = {
					_position.x - halfwidth, _position.y - _height,
					_position.x + halfwidth, _position.y - _height,
					_position.x - halfwidth, _position.y,
					_position.x - halfwidth, _position.y,
					_position.x + halfwidth, _position.y - _height,
					_position.x + halfwidth, _position.y
				};
				break;
			case ElementAnchor::Left:
				positions = {
					_position.x, _position.y - halfheight,
					_position.x + _width, _position.y - halfheight,
					_position.x, _position.y + halfheight,
					_position.x, _position.y + halfheight,
					_position.x + _width, _position.y - halfheight,
					_position.x + _width, _position.y + halfheight
				};
				break;
			case ElementAnchor::Right:
				positions = {
					_position.x - _width, _position.y - halfheight,
					_position.x, _position.y - halfheight,
					_position.x - _width, _position.y + halfheight,
					_position.x - _width, _position.y + halfheight,
					_position.x, _position.y - halfheight,
					_position.x, _position.y + halfheight
				};
				break;
			case ElementAnchor::Centre:
				positions = {
					_position.x - halfwidth, _position.y - halfheight,
					_position.x + halfwidth, _position.y - halfheight,
					_position.x - halfwidth, _position.y + halfheight,
					_position.x - halfwidth, _position.y + halfheight,
					_position.x + halfwidth, _position.y - halfheight,
					_position.x + halfwidth, _position.y + halfheight
				};
				break;
			} */
			
			// Create the mesh with the new data.
			_mesh = ElementMesh { positions, uvs };
		}
		
	}
	
}