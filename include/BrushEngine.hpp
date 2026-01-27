#ifndef BRUSH_ENGINE_HPP
#define BRUSH_ENGINE_HPP

#include <glad/glad.h>
#include <list>
#include "Common.hpp"

class BrushEngine {
public:
    BrushEngine();
    ~BrushEngine();
    
    void Initialize();
    void Render(const glm::mat4& viewProjection);
    
    void StartStroke(const glm::vec3& position);
    void ContinueStroke(const glm::vec3& position);
    void EndStroke();
    
    void SetBrushColor(const glm::vec4& color) { brushColor = color; }
    void SetBrushSize(float size) { brushSize = size; }
    
    glm::vec4 GetBrushColor() const { return brushColor; }
    float GetBrushSize() const { return brushSize; }

    void ClearAllStrokes();
    void UndoLastStroke();
    
    int GetStrokeCount() const { return strokes.size(); }
    
private:
    std::list<BrushStroke> strokes;
    BrushStroke* currentStroke;
    
    glm::vec4 brushColor;
    float brushSize;
    float minStrokeDistance;
    
    GLuint VAO, VBO;
    std::vector<Vertex> vertexBuffer;
    
    void CreateStrokeGeometry(BrushStroke& stroke, const glm::vec3& p1, const glm::vec3& p2);
    void UpdateGPUBuffer();
    void AddQuadToStroke(BrushStroke& stroke, const glm::vec3& p1, const glm::vec3& p2, float width);
};

#endif // BRUSH_ENGINE_HPP