#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <glad/glad.h>

#include "Common.hpp"
#include "VRSystem.hpp"
#include "ShaderManager.hpp"
#include "Canvas.hpp"
#include "BrushEngine.hpp"

class Renderer {
public:
    Renderer(VRSystem* vr);
    ~Renderer();
    
    bool Initialize();
    void Shutdown();
    
    void BeginFrame();
    void RenderScene(Canvas* canvas, BrushEngine* brush);
    void EndFrame();
    
    ShaderManager* GetShaderManager() { return &shaderManager; }
    
private:
    VRSystem* vrSystem;
    ShaderManager shaderManager;
    
    GLuint leftEyeFramebuffer;
    GLuint leftEyeTexture;
    GLuint leftEyeDepthBuffer;
    
    GLuint rightEyeFramebuffer;
    GLuint rightEyeTexture;
    GLuint rightEyeDepthBuffer;
    
    uint32_t renderWidth;
    uint32_t renderHeight;
    
    bool CreateFramebuffer(GLuint& framebuffer, GLuint& texture, GLuint& depthBuffer);
    void RenderEye(vr::EVREye eye, Canvas* canvas, BrushEngine* brush);
};

#endif // RENDERER_HPP