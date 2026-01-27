#include "../include/VRSystem.hpp"

VRSystem::VRSystem() 
    : vrSystem(nullptr)
    , hmdPose(1.0f)
    , renderWidth(0)
    , renderHeight(0)
    , leftControllerIndex(vr::k_unTrackedDeviceIndexInvalid)
    , rightControllerIndex(vr::k_unTrackedDeviceIndexInvalid) {}

VRSystem::~VRSystem() {
    Shutdown();
}

bool VRSystem::Initialize() {
    if (!vr::VR_IsRuntimeInstalled()) {
        LOG_ERROR("VR Runtime not installed");
        return false;
    }
    
    if (!vr::VR_IsHmdPresent()) {
        LOG_ERROR("HMD not detected");
        return false;
    }
    
    vr::EVRInitError error = vr::VRInitError_None;
    vrSystem = vr::VR_Init(&error, vr::VRApplication_Scene);
    
    if (error != vr::VRInitError_None) {
        LOG_ERROR("Failed to initialize VR: " << vr::VR_GetVRInitErrorAsEnglishDescription(error));
        return false;
    }
    
    vrSystem->GetRecommendedRenderTargetSize(&renderWidth, &renderHeight);
    LOG("VR System initialized. Render target: " << renderWidth << "x" << renderHeight);
    
    FindControllerIndices();
    
    return true;
}

void VRSystem::Shutdown() {
    if (vrSystem) {
        vr::VR_Shutdown();
        vrSystem = nullptr;
    }
}

void VRSystem::UpdatePoses() {
    if (!vrSystem) return;
    
    vr::VRCompositor()->WaitGetPoses(trackedDevicePose, vr::k_unMaxTrackedDeviceCount, nullptr, 0);
    
    if (trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].bPoseIsValid) {
        hmdPose = ConvertSteamVRMatrixToGLM(
            trackedDevicePose[vr::k_unTrackedDeviceIndex_Hmd].mDeviceToAbsoluteTracking
        );
    }
    
    if (leftControllerIndex != vr::k_unTrackedDeviceIndexInvalid && 
        trackedDevicePose[leftControllerIndex].bPoseIsValid) {
        leftController.pose = ConvertSteamVRMatrixToGLM(
            trackedDevicePose[leftControllerIndex].mDeviceToAbsoluteTracking
        );
    }
    
    if (rightControllerIndex != vr::k_unTrackedDeviceIndexInvalid && 
        trackedDevicePose[rightControllerIndex].bPoseIsValid) {
        rightController.pose = ConvertSteamVRMatrixToGLM(
            trackedDevicePose[rightControllerIndex].mDeviceToAbsoluteTracking
        );
    }
}

void VRSystem::UpdateControllerStates() {
    if (!vrSystem) return;
    
    if (leftControllerIndex != vr::k_unTrackedDeviceIndexInvalid) {
        vr::VRControllerState_t state;
        if (vrSystem->GetControllerState(leftControllerIndex, &state, sizeof(state))) {
            leftController.triggerPressed = (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) != 0;
            leftController.gripPressed = (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) != 0;
            leftController.touchpadPressed = (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) != 0;
            leftController.triggerValue = state.rAxis[1].x;
            leftController.touchpadPosition = glm::vec2(state.rAxis[0].x, state.rAxis[0].y);
        }
    }
    
    if (rightControllerIndex != vr::k_unTrackedDeviceIndexInvalid) {
        vr::VRControllerState_t state;
        if (vrSystem->GetControllerState(rightControllerIndex, &state, sizeof(state))) {
            rightController.triggerPressed = (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Trigger)) != 0;
            rightController.gripPressed = (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_Grip)) != 0;
            rightController.touchpadPressed = (state.ulButtonPressed & vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad)) != 0;
            rightController.triggerValue = state.rAxis[1].x;
            rightController.touchpadPosition = glm::vec2(state.rAxis[0].x, state.rAxis[0].y);
        }
    }
}

glm::mat4 VRSystem::GetProjectionMatrix(vr::EVREye eye, float nearClip, float farClip) const {
    if (!vrSystem) return glm::mat4(1.0f);
    return ConvertSteamVRMatrixToGLM(vrSystem->GetProjectionMatrix(eye, nearClip, farClip));
}

glm::mat4 VRSystem::GetEyeToHeadTransform(vr::EVREye eye) const {
    if (!vrSystem) return glm::mat4(1.0f);
    return glm::inverse(ConvertSteamVRMatrixToGLM(vrSystem->GetEyeToHeadTransform(eye)));
}

ControllerState VRSystem::GetControllerState(vr::ETrackedControllerRole role) const {
    if (role == vr::TrackedControllerRole_LeftHand) {
        return leftController;
    } else {
        return rightController;
    }
}

void VRSystem::TriggerHapticPulse(vr::ETrackedControllerRole role, unsigned short duration) {
    if (!vrSystem) return;
    
    vr::TrackedDeviceIndex_t index = (role == vr::TrackedControllerRole_LeftHand) 
        ? leftControllerIndex : rightControllerIndex;
    
    if (index != vr::k_unTrackedDeviceIndexInvalid) {
        vrSystem->TriggerHapticPulse(index, 0, duration);
    }
}

void VRSystem::SubmitFrameToHMD(unsigned int leftEyeTexture, unsigned int rightEyeTexture) {
    if (!vrSystem) return;
    
    vr::Texture_t leftEye = { (void*)(uintptr_t)leftEyeTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    vr::Texture_t rightEye = { (void*)(uintptr_t)rightEyeTexture, vr::TextureType_OpenGL, vr::ColorSpace_Gamma };
    
    vr::VRCompositor()->Submit(vr::Eye_Left, &leftEye);
    vr::VRCompositor()->Submit(vr::Eye_Right, &rightEye);
}

glm::mat4 VRSystem::ConvertSteamVRMatrixToGLM(const vr::HmdMatrix34_t& matrix) const {
    return glm::mat4(
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], 0.0f,
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], 0.0f,
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], 0.0f,
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], 1.0f
    );
}

glm::mat4 VRSystem::ConvertSteamVRMatrixToGLM(const vr::HmdMatrix44_t& matrix) const {
    return glm::mat4(
        matrix.m[0][0], matrix.m[1][0], matrix.m[2][0], matrix.m[3][0],
        matrix.m[0][1], matrix.m[1][1], matrix.m[2][1], matrix.m[3][1],
        matrix.m[0][2], matrix.m[1][2], matrix.m[2][2], matrix.m[3][2],
        matrix.m[0][3], matrix.m[1][3], matrix.m[2][3], matrix.m[3][3]
    );
}

void VRSystem::FindControllerIndices() {
    if (!vrSystem) return;
    
    leftControllerIndex = vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_LeftHand);
    rightControllerIndex = vrSystem->GetTrackedDeviceIndexForControllerRole(vr::TrackedControllerRole_RightHand);
    
    LOG("Left controller index: " << leftControllerIndex);
    LOG("Right controller index: " << rightControllerIndex);
}
