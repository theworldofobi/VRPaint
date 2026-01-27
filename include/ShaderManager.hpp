#ifndef SHADER_MANAGER_HPP
#define SHADER_MANAGER_HPP

#include <glad/glad.h>
#include <map>
#include "Common.hpp"

class ShaderManager {
public:
    ShaderManager();
    ~ShaderManager();
    
    bool LoadShader(const std::string& name, 
                    const std::string& vertexPath, 
                    const std::string& fragmentPath);
    
    void UseShader(const std::string& name);
    GLuint GetShaderProgram(const std::string& name) const;
    
    // uniform setters
    void SetMat4(const std::string& shaderName, const std::string& uniformName, const glm::mat4& value);
    void SetVec3(const std::string& shaderName, const std::string& uniformName, const glm::vec3& value);
    void SetVec4(const std::string& shaderName, const std::string& uniformName, const glm::vec4& value);
    void SetFloat(const std::string& shaderName, const std::string& uniformName, float value);
    void SetInt(const std::string& shaderName, const std::string& uniformName, int value);
    
private:
    std::map<std::string, GLuint> shaders;
    
    std::string ReadShaderFile(const std::string& filepath);
    bool CompileShader(GLuint shader, const std::string& source);
    bool LinkProgram(GLuint program);
};

#endif // SHADER_MANAGER_HPP