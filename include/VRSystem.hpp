#ifndef VRSYSTEM_HPP
#define VRSYSTEM_HPP

#include <openvr.h>
#include "Common.hpp"

class VRSystem {
public:
    VRSystem();
    ~VRSystem();
    
    bool Initialize();
    void Shutdown();
    
    void UpdatePoses();
    void UpdateControllerStates();
    
    glm::mat4 GetHMDPose() const { return hmdPose; }
    glm::mat4 GetProjectionMatrix(vr::EVREye eye, float nearClip, float farClip) const;
    glm::mat4 GetEyeToHeadTransform(vr::EVREye eye) const;
    
    ControllerState GetControllerState(vr::ETrackedControllerRole role) const;
    void TriggerHapticPulse(vr::ETrackedControllerRole role, unsigned short duration = 1000);
    
    bool IsHMDPresent() const { return vrSystem != nullptr; }
    uint32_t GetRenderWidth() const { return renderWidth; }
    uint32_t GetRenderHeight() const { return renderHeight; }
    
    void SubmitFrameToHMD(unsigned int leftEyeTexture, unsigned int rightEyeTexture);
    
private:
    vr::IVRSystem* vrSystem;
    vr::TrackedDevicePose_t trackedDevicePose[vr::k_unMaxTrackedDeviceCount];
    
    glm::mat4 hmdPose;
    ControllerState leftController;
    ControllerState rightController;
    
    uint32_t renderWidth;
    uint32_t renderHeight;
    
    vr::TrackedDeviceIndex_t leftControllerIndex;
    vr::TrackedDeviceIndex_t rightControllerIndex;
    
    glm::mat4 ConvertSteamVRMatrixToGLM(const vr::HmdMatrix34_t& matrix) const;
    glm::mat4 ConvertSteamVRMatrixToGLM(const vr::HmdMatrix44_t& matrix) const;
    void FindControllerIndices();
};

#endif // VRSYSTEM_HPP