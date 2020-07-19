#include "ui/Manager.hpp"

namespace arterra {
	
	namespace UI {
		
		Manager::Manager(ShaderManager* shaderManager, Renderer* renderer, sf::Event &windowEvent)
			: _shaderManager(shaderManager), _renderer(renderer), _elements(), _event(&windowEvent) {}
		
		void Manager::CreateElement(std::string identifier, Element element) {
			// First run copy constructor.
			_elements[identifier] = element;
			
			// Then create the mesh and collider on the "proper" copy.
			_elements[identifier].CreateMesh();
			_elements[identifier].CreateCollider();
		}
		
		void Manager::DestroyElement(std::string identifier) {
			// TODO: first delete element data
			auto element = _elements.find(identifier);
			if (element == _elements.end()) {
				Logger::Get().Log(Logger::Warning, "Trying to destroy UI element `", identifier,
					"' which does not exist!");
				return;
			}
			_elements.erase(element);
		}
		
		Element* Manager::GetElement(std::string identifier) {
			auto element = _elements.find(identifier);
			if (element == _elements.end()) {
				Logger::Get().Log(Logger::Warning, "Trying to access UI element `", identifier,
					"' which does not exist!");
			}
			return &element->second;
		}
		
		void Manager::Update() {
			if (_event->type == sf::Event::MouseMoved) {
				glm::vec2 mousePos = glm::vec2 { _event->mouseMove.x, (720-_event->mouseMove.y) };
				for (auto it = _elements.begin(); it != _elements.end(); it++) {
					if (it->second.GetCollider()->ContainsPoint(mousePos) == true) {
						it->second.GetMaterial().UpdateParameter("u_ColourTint", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
					}else {
						it->second.GetMaterial().UpdateParameter("u_ColourTint", glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					}
				}
			}
			
		}
		
		void Manager::Render() {
			for (auto it = _elements.begin(); it != _elements.end(); it++ ){
				// Disable depth test.
				glDisable(GL_DEPTH_TEST);
				
				// Update transform.
				it->second.UpdateTransform();
				
				// Get all shader parameters for this gui-element's material.
				auto parameters = it->second.GetMaterial().GetParameters();
				_shaderManager->UseShader("gui-fancy");
				for (auto param: parameters) {
					switch (param._type)
					{
					case UI::ShaderParameter::Type::Int :
						_shaderManager->ActiveProgram().SetUniform(param._name, param._value.i);
						break;
					case UI::ShaderParameter::Type::Float :
						_shaderManager->ActiveProgram().SetUniform(param._name, param._value.f);
						break;
					case UI::ShaderParameter::Type::Vec2 :
						_shaderManager->ActiveProgram().SetUniform(param._name, param._value.v2);
						break;
					case UI::ShaderParameter::Type::Vec3 :
						_shaderManager->ActiveProgram().SetUniform(param._name, param._value.v3);
						break;
					case UI::ShaderParameter::Type::Vec4 :
						_shaderManager->ActiveProgram().SetUniform(param._name, param._value.v4);
						break;
					case UI::ShaderParameter::Type::Mat4 :
						_shaderManager->ActiveProgram().SetUniform(param._name, param._value.m4);
						break;
					}
				}
				
				// Bind and draw the gui-element.
				it->second.GetMesh().Bind();
				_renderer->DrawTriangles(it->second.GetMesh().GetVertexCount());
				
			}
		}
		
	}
	
}