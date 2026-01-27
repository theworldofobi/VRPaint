#ifndef COMMON_HPP
#define COMMON_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <memory>
#include <string>
#include <iostream>

// quick loggers
#define LOG(x) std::cout << x << std::endl
#define LOG_ERROR(x) std::cerr << "ERROR: " << x << std::endl

struct Vertex {
    glm::vec3 position;
    glm::vec4 color;
    
    Vertex() : position(0.0f), color(1.0f) {}
    Vertex(const glm::vec3& pos, const glm::vec4& col) 
        : position(pos)
        , color(col) {}
};

struct BrushStroke {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    float width;
    glm::vec4 color;
    bool completed;
    
    BrushStroke() : width(0.01f)
                  , color(1.0f)
                  , completed(false) {}
};

struct ControllerState {
    glm::mat4 pose;
    bool triggerPressed;
    float triggerValue;
    bool gripPressed;
    bool touchpadPressed;
    glm::vec2 touchpadPosition;
    
    ControllerState() : pose(1.0f)
                      , triggerPressed(false)
                      , triggerValue(0.0f)
                      , gripPressed(false)
                      , touchpadPressed(false)
                      , touchpadPosition(0.0f) {}
};

inline glm::vec4 HSVtoRGB(float h, float s, float v, float a = 1.0f) {
    float c = v * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    
    float r, g, b;
    if (h < 60.0f) { r = c; g = x; b = 0; }
    else if (h < 120.0f) { r = x; g = c; b = 0; }
    else if (h < 180.0f) { r = 0; g = c; b = x; }
    else if (h < 240.0f) { r = 0; g = x; b = c; }
    else if (h < 300.0f) { r = x; g = 0; b = c; }
    else { r = c; g = 0; b = x; }
    
    return glm::vec4(r + m, g + m, b + m, a);
}

// ray-plane intersection for canvas interaction
inline bool RayPlaneIntersection(const glm::vec3& rayOrigin, 
                                 const glm::vec3& rayDir,
                                 const glm::vec3& planePoint,
                                 const glm::vec3& planeNormal,
                                 glm::vec3& hitPoint) {
    float denom = glm::dot(planeNormal, rayDir);
    if (std::abs(denom) < 0.0001f) return false;
    
    float t = glm::dot(planePoint - rayOrigin, planeNormal) / denom;
    if (t < 0) return false;
    
    hitPoint = rayOrigin + rayDir * t;
    return true;
}

#endif // COMMON_HPP