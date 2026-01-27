#ifndef CANVAS_HPP
#define CANVAS_HPP

#include <glad/glad.h>

#include "Common.hpp"

class Canvas {
public:
    Canvas();
    ~Canvas();
    
    void Initialize(float width = 2.0f, float height = 1.5f);
    void Render(const glm::mat4& viewProjection);
    
    glm::vec3 GetPosition() const { return position; }
    glm::vec3 GetNormal() const { return normal; }
    
    void SetPosition(const glm::vec3& pos) { position = pos; UpdateModelMatrix(); }
    void SetRotation(const glm::vec3& rot) { rotation = rot; UpdateModelMatrix(); }
    
    glm::mat4 GetModelMatrix() const { return modelMatrix; }
    
    // Check if a point is within canvas bounds (in canvas local space)
    bool IsPointInBounds(const glm::vec3& point) const;
    
    // Convert world space point to canvas local UV coordinates
    glm::vec2 WorldPointToCanvasUV(const glm::vec3& worldPoint) const;
    
private:
    GLuint VAO, VBO, EBO;
    
    float width, height;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 normal;
    glm::mat4 modelMatrix;
    
    void SetupMesh();
    void UpdateModelMatrix();
};

#endif // CANVAS_HPP