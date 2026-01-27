#include "../include/BrushEngine.hpp"

BrushEngine::BrushEngine()
    : currentStroke(nullptr)
    , brushColor(0.0f, 0.0f, 0.0f, 1.0f)
    , brushSize(0.01f)
    , minStrokeDistance(0.005f)
    , VAO(0)
    , VBO(0) {}

BrushEngine::~BrushEngine() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
}

void BrushEngine::Initialize() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void BrushEngine::StartStroke(const glm::vec3& position) {
    strokes.push_back(BrushStroke());
    currentStroke = &strokes.back();
    currentStroke->color = brushColor;
    currentStroke->width = brushSize;

    currentStroke->vertices.push_back(Vertex(position, brushColor));
}

void BrushEngine::ContinueStroke(const glm::vec3& position) {
    if (!currentStroke || currentStroke->vertices.empty()) return;
    
    const glm::vec3& lastPos = currentStroke->vertices.back().position;
    float distance = glm::distance(position, lastPos);
    
    if (distance < minStrokeDistance) return;
    
    // to be implemented
    CreateStrokeGeometry(*currentStroke, lastPos, position);
    
    currentStroke->vertices.push_back(Vertex(position, brushColor));
}

void BrushEngine::EndStroke() {
    if (currentStroke) {
        currentStroke->completed = true;
        currentStroke = nullptr;
        UpdateGPUBuffer();
    }
}

void BrushEngine::ClearAllStrokes() {
    strokes.clear();
    currentStroke = nullptr;
    vertexBuffer.clear();
    UpdateGPUBuffer();
}

void BrushEngine::UndoLastStroke() {
    if (!strokes.empty()) {
        strokes.pop_back();
        currentStroke = nullptr;
        
        vertexBuffer.clear();
        for (auto& stroke : strokes) {
            vertexBuffer.insert(vertexBuffer.end(), 
                                stroke.vertices.begin(), 
                                stroke.vertices.end());
        }
        UpdateGPUBuffer();
    }
}

void BrushEngine::Render(const glm::mat4& viewProjection) {
    if (vertexBuffer.empty() && (!currentStroke || currentStroke->vertices.empty())) return;
    
    std::vector<Vertex> renderBuffer = vertexBuffer;
    
    if (currentStroke && !currentStroke->vertices.empty()) {
        renderBuffer.insert(renderBuffer.end(), 
                           currentStroke->vertices.begin(), 
                           currentStroke->vertices.end());
    }
    
    if (renderBuffer.empty()) return;
    
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, renderBuffer.size() * sizeof(Vertex), 
                 renderBuffer.data(), GL_DYNAMIC_DRAW);
    
    glLineWidth(3.0f);
    glDrawArrays(GL_LINE_STRIP, 0, renderBuffer.size());
    
    glBindVertexArray(0);
}

void BrushEngine::CreateStrokeGeometry(BrushStroke& stroke, const glm::vec3& p1, const glm::vec3& p2) {
    // to be implemented, using line rendering for now, quad geometry/billboarded quads to come
    return;
}

void BrushEngine::UpdateGPUBuffer() {
    vertexBuffer.clear();
    
    for (const auto& stroke : strokes) {
        if (stroke.completed) {
            vertexBuffer.insert(vertexBuffer.end(), stroke.vertices.begin(), stroke.vertices.end());
        }
    }
}

void BrushEngine::AddQuadToStroke(BrushStroke& stroke, const glm::vec3& p1, const glm::vec3& p2, float width) {
    glm::vec3 direction = glm::normalize(p2 - p1);
    glm::vec3 up = glm::vec3(0, 1, 0);
    
    if (std::abs(glm::dot(direction, up)) > 0.99f) {
        up = glm::vec3(1, 0, 0);
    }
    
    glm::vec3 right = glm::normalize(glm::cross(direction, up)) * width * 0.5f;
    
    glm::vec3 v1 = p1 - right;
    glm::vec3 v2 = p1 + right;
    glm::vec3 v3 = p2 + right;
    glm::vec3 v4 = p2 - right;
    
    stroke.vertices.push_back(Vertex(v1, brushColor));
    stroke.vertices.push_back(Vertex(v2, brushColor));
    stroke.vertices.push_back(Vertex(v3, brushColor));
    
    stroke.vertices.push_back(Vertex(v1, brushColor));
    stroke.vertices.push_back(Vertex(v3, brushColor));
    stroke.vertices.push_back(Vertex(v4, brushColor));
}
