#pragma once
#include "PCH.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>

namespace arterra {

    class ShaderProgram {

        struct Shader {
            enum ShaderType {Fragment, Vertex, Geometry};
            ShaderType _shaderType;
            GLuint _ID;

            Shader(ShaderType shaderType, std::string path) : _shaderType {shaderType} {
                std::string src = ReadShaderFile(path);
                const char *cSrc = src.data();
                switch (shaderType) {
                    case ShaderType::Fragment:
                        _ID = glCreateShader(GL_FRAGMENT_SHADER);
                    break;
                    case ShaderType::Vertex:
                        _ID = glCreateShader(GL_VERTEX_SHADER);
                    break;
                    case ShaderType::Geometry:
                        _ID = glCreateShader(GL_GEOMETRY_SHADER);
                    break;
                }

                glShaderSource(_ID, 1, &cSrc, nullptr);
                glCompileShader(_ID);

            }
        };

        public:
            ShaderProgram() = default;

            // Create a new shader (RAII)
            ShaderProgram(std::string vertShaderPath, std::string fragShaderPath) {
                Create(vertShaderPath, fragShaderPath);
            };

            // Delete the program
            ~ShaderProgram() {
                glDeleteProgram(_shaderProgram);
            }

            // Create a new shader - return whether or not it worked
            bool Create(std::string vertPath, std::string fragPath);

            // Inline returns the program handle
            inline GLuint GetProgram() const { return _shaderProgram; }


            // Inlines for setting uniforms in shaders
            inline GLint GetUniform(const char *name) {
                return glGetUniformLocation(_shaderProgram, name);
            }

            inline void SetUniform(std::string uniformName, glm::mat4 matrix) {
                auto uniform = GetUniform(uniformName.c_str());
                glUniformMatrix4fv(uniform, 1, GL_FALSE, glm::value_ptr(matrix));
            }

            inline void SetUniform(std::string uniformName, float_t value) {
                auto uniform = GetUniform(uniformName.c_str());
                glUniform1f(uniform, value);
            }

            inline void SetUniform(std::string uniformName, double_t value) {
                auto uniform = GetUniform(uniformName.c_str());
                glUniform1d(uniform, value);
            }

            inline void SetUniform(std::string uniformName, glm::vec3 vector) {
                auto uniform = GetUniform(uniformName.c_str());
                glUniform3fv(uniform, 1, glm::value_ptr(vector));
            } 


        private:
            static std::string ReadShaderFile(std::string path);
            static bool CheckShaderCompilation(GLuint shader);

            std::vector<Shader> _shaders;
            GLuint _shaderProgram;
    };

};