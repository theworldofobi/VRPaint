#include "../include/ShaderManager.hpp"
#include <fstream>
#include <sstream>

ShaderManager::ShaderManager() {}

ShaderManager::~ShaderManager() {
    for (auto& pair : shaders) {
        glDeleteProgram(pair.second);
    }
}

bool ShaderManager::LoadShader(const std::string& name, 
                                const std::string& vertexPath, 
                                const std::string& fragmentPath) {

    std::string vertexCode = ReadShaderFile(vertexPath);
    std::string fragmentCode = ReadShaderFile(fragmentPath);
    
    if (vertexCode.empty() || fragmentCode.empty()) {
        LOG_ERROR("Failed to read shader files for: " << name);
        return false;
    }

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    if (!CompileShader(vertexShader, vertexCode)) {
        glDeleteShader(vertexShader);
        return false;
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    if (!CompileShader(fragmentShader, fragmentCode)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return false;
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    
    if (!LinkProgram(program)) {
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(program);
        return false;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    shaders[name] = program;
    LOG("Loaded shader: " << name);
    
    return true;
}

void ShaderManager::UseShader(const std::string& name) {
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        glUseProgram(it->second);
    } else {
        LOG_ERROR("Shader not found: " << name);
    }
}

GLuint ShaderManager::GetShaderProgram(const std::string& name) const {
    auto it = shaders.find(name);
    return (it != shaders.end()) ? it->second : 0;
}

void ShaderManager::SetMat4(const std::string& shaderName, const std::string& uniformName, const glm::mat4& value) {
    GLuint program = GetShaderProgram(shaderName);
    if (program) {
        GLint location = glGetUniformLocation(program, uniformName.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void ShaderManager::SetVec3(const std::string& shaderName, const std::string& uniformName, const glm::vec3& value) {
    GLuint program = GetShaderProgram(shaderName);
    if (program) {
        GLint location = glGetUniformLocation(program, uniformName.c_str());
        glUniform3fv(location, 1, glm::value_ptr(value));
    }
}

void ShaderManager::SetVec4(const std::string& shaderName, const std::string& uniformName, const glm::vec4& value) {
    GLuint program = GetShaderProgram(shaderName);
    if (program) {
        GLint location = glGetUniformLocation(program, uniformName.c_str());
        glUniform4fv(location, 1, glm::value_ptr(value));
    }
}

void ShaderManager::SetFloat(const std::string& shaderName, const std::string& uniformName, float value) {
    GLuint program = GetShaderProgram(shaderName);
    if (program) {
        GLint location = glGetUniformLocation(program, uniformName.c_str());
        glUniform1f(location, value);
    }
}

void ShaderManager::SetInt(const std::string& shaderName, const std::string& uniformName, int value) {
    GLuint program = GetShaderProgram(shaderName);
    if (program) {
        GLint location = glGetUniformLocation(program, uniformName.c_str());
        glUniform1i(location, value);
    }
}

std::string ShaderManager::ReadShaderFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        LOG_ERROR("Failed to open shader file: " << filepath);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool ShaderManager::CompileShader(GLuint shader, const std::string& source) {
    const char* sourcePtr = source.c_str();
    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);
    
    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        LOG_ERROR("Shader compilation failed: " << infoLog);
        return false;
    }
    
    return true;
}

bool ShaderManager::LinkProgram(GLuint program) {
    glLinkProgram(program);
    
    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        LOG_ERROR("Program linking failed: " << infoLog);
        return false;
    }
    
    return true;
}
