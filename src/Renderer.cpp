#include "../include/Renderer.hpp"

Renderer::Renderer(VRSystem* vr)
    : vrSystem(vr)
    , leftEyeFramebuffer(0)
    , leftEyeTexture(0)
    , leftEyeDepthBuffer(0)
    , rightEyeFramebuffer(0)
    , rightEyeTexture(0)
    , rightEyeDepthBuffer(0)
    , renderWidth(0)
    , renderHeight(0) {}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize() {
    if (!vrSystem || !vrSystem->IsHMDPresent()) {
        LOG_ERROR("VR System not available");
        return false;
    }
    
    renderWidth = vrSystem->GetRenderWidth();
    renderHeight = vrSystem->GetRenderHeight();
    
    LOG("Initializing renderer with resolution: " << renderWidth << "x" << renderHeight);
    
    if (!shaderManager.LoadShader("basic", "shaders/basic.vert", "shaders/basic.frag")) {
        LOG_ERROR("Failed to load basic shader");
        return false;
    }
    
    if (!CreateFramebuffer(leftEyeFramebuffer, leftEyeTexture, leftEyeDepthBuffer)) {
        LOG_ERROR("Failed to create left eye framebuffer");
        return false;
    }
    
    if (!CreateFramebuffer(rightEyeFramebuffer, rightEyeTexture, rightEyeDepthBuffer)) {
        LOG_ERROR("Failed to create right eye framebuffer");
        return false;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    LOG("Renderer initialized successfully");
    return true;
}

void Renderer::Shutdown() {
    if (leftEyeFramebuffer) {
        glDeleteFramebuffers(1, &leftEyeFramebuffer);
        glDeleteTextures(1, &leftEyeTexture);
        glDeleteRenderbuffers(1, &leftEyeDepthBuffer);
    }
    
    if (rightEyeFramebuffer) {
        glDeleteFramebuffers(1, &rightEyeFramebuffer);
        glDeleteTextures(1, &rightEyeTexture);
        glDeleteRenderbuffers(1, &rightEyeDepthBuffer);
    }
}

bool Renderer::CreateFramebuffer(GLuint& framebuffer, GLuint& texture, GLuint& depthBuffer) {
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, renderWidth, renderHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, renderWidth, renderHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    if (status != GL_FRAMEBUFFER_COMPLETE) {
        LOG_ERROR("Framebuffer incomplete: " << status);
        return false;
    }
    
    return true;
}

void Renderer::BeginFrame() {
    vrSystem->UpdatePoses();
}

void Renderer::RenderScene(Canvas* canvas, BrushEngine* brush) {
    RenderEye(vr::Eye_Left, canvas, brush);
    RenderEye(vr::Eye_Right, canvas, brush);
}

void Renderer::RenderEye(vr::EVREye eye, Canvas* canvas, BrushEngine* brush) {
    GLuint framebuffer = (eye == vr::Eye_Left) ? leftEyeFramebuffer : rightEyeFramebuffer;
    
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, renderWidth, renderHeight);
    
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glm::mat4 hmdPose = vrSystem->GetHMDPose();
    glm::mat4 eyeToHead = vrSystem->GetEyeToHeadTransform(eye);
    glm::mat4 projection = vrSystem->GetProjectionMatrix(eye, 0.1f, 100.0f);
    
    glm::mat4 view = glm::inverse(hmdPose * eyeToHead);
    glm::mat4 viewProjection = projection * view;
    
    shaderManager.UseShader("basic");

    shaderManager.SetMat4("basic", "mvp", viewProjection * canvas->GetModelMatrix());
    canvas->Render(viewProjection);
    
    shaderManager.SetMat4("basic", "mvp", viewProjection);
    brush->Render(viewProjection);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::EndFrame() {
    vrSystem->SubmitFrameToHMD(leftEyeTexture, rightEyeTexture);
}
