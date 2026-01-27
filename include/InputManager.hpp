#ifndef INPUT_MANAGER_HPP
#define INPUT_MANAGER_HPP

#include "Common.hpp"
#include "VRSystem.hpp"
#include "Canvas.hpp"
#include "BrushEngine.hpp"

class InputManager {
public:
    InputManager(VRSystem* vr, Canvas* canvas, BrushEngine* brush);
    
    void Update();
    void RenderControllers(const glm::mat4& viewProjection);
    
private:
    VRSystem* vrSystem;
    Canvas* canvas;
    BrushEngine* brushEngine;
    
    bool wasPainting;
    glm::vec3 lastPaintPosition;
    
    glm::vec3 GetControllerRayOrigin(const ControllerState& controller);
    glm::vec3 GetControllerRayDirection(const ControllerState& controller);
    
    float currentHue;
    float currentSaturation;
    float currentValue;
    
    void HandlePaintingInput(const ControllerState& controller);
    void HandleColorPickerInput(const ControllerState& controller);
    void HandleBrushSizeInput(const ControllerState& controller);
};

#endif // INPUT_MANAGER_HPP