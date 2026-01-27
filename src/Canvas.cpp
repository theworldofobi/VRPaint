#include "../include/Canvas.hpp"

Canvas::Canvas() 
    : VAO(0)
    , VBO(0)
    , EBO(0)
    , width(2.0f)
    , height(1.5f)
    , position(0.0f, 1.5f, -2.0f)
    , rotation(0.0f)
    , normal(0.0f, 0.0f, 1.0f)
    , modelMatrix(1.0f) {}

Canvas::~Canvas() {
    if (VAO) glDeleteVertexArrays(1, &VAO);
    if (VBO) glDeleteBuffers(1, &VBO);
    if (EBO) glDeleteBuffers(1, &EBO);
}

void Canvas::Initialize(float w, float h) {
    width = w;
    height = h;
    SetupMesh();
    UpdateModelMatrix();
}

void Canvas::SetupMesh() {
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    float vertices[] = {
        // positions              // polors (white is default)
        -halfWidth, -halfHeight, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
         halfWidth, -halfHeight, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
         halfWidth,  halfHeight, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f,
        -halfWidth,  halfHeight, 0.0f,  1.0f, 1.0f, 1.0f, 1.0f
    };
    
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
}

void Canvas::Render(const glm::mat4& viewProjection) {
    glm::mat4 mvp = viewProjection * modelMatrix;
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Canvas::UpdateModelMatrix() {
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    
    glm::vec4 transformedNormal = modelMatrix * glm::vec4(0, 0, 1, 0);
    normal = glm::normalize(glm::vec3(transformedNormal));
}

bool Canvas::IsPointInBounds(const glm::vec3& point) const {
    glm::mat4 invModel = glm::inverse(modelMatrix);
    glm::vec4 localPoint = invModel * glm::vec4(point, 1.0f);
    
    float halfWidth = width / 2.0f;
    float halfHeight = height / 2.0f;
    
    return (std::abs(localPoint.x) <= halfWidth && 
            std::abs(localPoint.y) <= halfHeight &&
            std::abs(localPoint.z) < 0.1f);
}

glm::vec2 Canvas::WorldPointToCanvasUV(const glm::vec3& worldPoint) const {
    glm::mat4 invModel = glm::inverse(modelMatrix);
    glm::vec4 localPoint = invModel * glm::vec4(worldPoint, 1.0f);
    
    float u = (localPoint.x / width) + 0.5f;
    float v = (localPoint.y / height) + 0.5f;
    
    return glm::vec2(u, v);
}
