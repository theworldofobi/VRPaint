#include "../include/InputManager.hpp"

InputManager::InputManager(VRSystem* vr, Canvas* cnv, BrushEngine* brush)
    : vrSystem(vr)
    , canvas(cnv)
    , brushEngine(brush)
    , wasPainting(false)
    , currentHue(0.0f)
    , currentSaturation(1.0f)
    , currentValue(1.0f) {
}

void InputManager::Update() {
    if (!vrSystem || !canvas || !brushEngine) return;
    
    ControllerState rightController = vrSystem->GetControllerState(vr::TrackedControllerRole_RightHand);
    ControllerState leftController = vrSystem->GetControllerState(vr::TrackedControllerRole_LeftHand);
    
    HandlePaintingInput(rightController);
    HandleColorPickerInput(leftController);
    HandleBrushSizeInput(leftController);
    
    static bool wasGripPressed = false;
    if (leftController.gripPressed && !wasGripPressed) {
        brushEngine->UndoLastStroke();
        vrSystem->TriggerHapticPulse(vr::TrackedControllerRole_LeftHand, 500);
    }
    wasGripPressed = leftController.gripPressed;
    
    static bool wasBothGripsPressed = false;
    if (leftController.gripPressed && rightController.gripPressed && !wasBothGripsPressed) {
        brushEngine->ClearAllStrokes();
        vrSystem->TriggerHapticPulse(vr::TrackedControllerRole_LeftHand, 1000);
        vrSystem->TriggerHapticPulse(vr::TrackedControllerRole_RightHand, 1000);
    }
    wasBothGripsPressed = leftController.gripPressed && rightController.gripPressed;
}

void InputManager::HandlePaintingInput(const ControllerState& controller) {
    if (!controller.triggerPressed) {
        if (wasPainting) {
            brushEngine->EndStroke();
            wasPainting = false;
        }
        return;
    }

    glm::vec3 rayOrigin = GetControllerRayOrigin(controller);
    glm::vec3 rayDir = GetControllerRayDirection(controller);
    glm::vec3 hitPoint;
    
    if (RayPlaneIntersection(rayOrigin, rayDir, canvas->GetPosition(), canvas->GetNormal(), hitPoint)) {
        if (canvas->IsPointInBounds(hitPoint)) {
            if (!wasPainting) {
                brushEngine->StartStroke(hitPoint);
                wasPainting = true;
                vrSystem->TriggerHapticPulse(vr::TrackedControllerRole_RightHand, 300);
            } else {
                brushEngine->ContinueStroke(hitPoint);

                if (glm::distance(hitPoint, lastPaintPosition) > 0.01f) {
                    vrSystem->TriggerHapticPulse(vr::TrackedControllerRole_RightHand, 100);
                    lastPaintPosition = hitPoint;
                }
            }
        } else if (wasPainting) {
            brushEngine->EndStroke();
            wasPainting = false;
        }
    } else if (wasPainting) {
        brushEngine->EndStroke();
        wasPainting = false;
    }
}

void InputManager::HandleColorPickerInput(const ControllerState& controller) {
    if (!controller.touchpadPressed) return;
    
    currentHue = (controller.touchpadPosition.x + 1.0f) * 180.0f; // 0-360 degrees
    currentSaturation = (controller.touchpadPosition.y + 1.0f) * 0.5f; // 0-1
    currentSaturation = glm::clamp(currentSaturation, 0.0f, 1.0f);
    
    glm::vec4 newColor = HSVtoRGB(currentHue, currentSaturation, currentValue);
    brushEngine->SetBrushColor(newColor);
    
    static glm::vec4 lastColor(0.0f);
    if (glm::distance(newColor, lastColor) > 0.1f) {
        vrSystem->TriggerHapticPulse(vr::TrackedControllerRole_LeftHand, 50);
        lastColor = newColor;
    }
}

void InputManager::HandleBrushSizeInput(const ControllerState& controller) {
    if (controller.triggerValue > 0.1f) {
        float newSize = 0.005f + (controller.triggerValue * 0.045f); // Range: 0.005 to 0.05
        brushEngine->SetBrushSize(newSize);
    }
}

glm::vec3 InputManager::GetControllerRayOrigin(const ControllerState& controller) {
    return glm::vec3(controller.pose[3]);
}

glm::vec3 InputManager::GetControllerRayDirection(const ControllerState& controller) {
    glm::vec3 forward = glm::vec3(controller.pose[2]);
    return -glm::normalize(forward);
}

void InputManager::RenderControllers(const glm::mat4& viewProjection) {
    // to be implemented, controller mesh rendering
    // controllers should be visible through SteamVR overlay
    return;
}
