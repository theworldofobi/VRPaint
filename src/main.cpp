#include "VRSystem.hpp"
#include "Renderer.hpp"
#include "Canvas.hpp"
#include "BrushEngine.hpp"
#include "InputManager.hpp"
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>

void glfwErrorCallback(int error, const char* description) {
    LOG_ERROR("GLFW Error " << error << ": " << description);
}

int main(int argc, char* argv[]) {
    LOG("Starting VRPaint...");
    
    glfwSetErrorCallback(glfwErrorCallback);
    if (!glfwInit()) {
        LOG_ERROR("Failed to initialize GLFW");
        return -1;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "VR Paint", nullptr, nullptr);
    if (!window) {
        LOG_ERROR("Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    VRSystem vrSystem;
    if (!vrSystem.Initialize()) {
        LOG_ERROR("Failed to initialize VR system");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    Renderer renderer(&vrSystem);
    if (!renderer.Initialize()) {
        LOG_ERROR("Failed to initialize renderer");
        vrSystem.Shutdown();
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    
    Canvas canvas;
    canvas.Initialize(2.0f, 1.5f);
    canvas.SetPosition(glm::vec3(0.0f, 1.5f, -2.0f));
    
    BrushEngine brushEngine;
    brushEngine.Initialize();
    brushEngine.SetBrushColor(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    brushEngine.SetBrushSize(0.01f);

    InputManager inputManager(&vrSystem, &canvas, &brushEngine);
    
    LOG("All systems initialized. Starting main loop...");
    LOG("");
    LOG("=== VR Paint Controls ===");
    LOG("Right Controller Trigger: Paint on canvas");
    LOG("Left Controller Touchpad: Change color (X=Hue, Y=Saturation)");
    LOG("Left Controller Trigger: Adjust brush size");
    LOG("Left Controller Grip: Undo last stroke");
    LOG("Both Controller Grips: Clear all strokes");
    LOG("");
    
    auto lastTime = std::chrono::high_resolution_clock::now();
    int frameCount = 0;
    
    while (!glfwWindowShouldClose(window)) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();
        
        frameCount++;
        if (deltaTime >= 1000) {
            float fps = frameCount / (deltaTime / 1000.0f);
            LOG("FPS: " << fps << " | Strokes: " << brushEngine.GetStrokeCount());
            frameCount = 0;
            lastTime = currentTime;
        }
        
        vrSystem.UpdateControllerStates();
        
        inputManager.Update();
        
        renderer.BeginFrame();
        renderer.RenderScene(&canvas, &brushEngine);
        renderer.EndFrame();
        
        glfwPollEvents();
        
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    
    LOG("Shutting down...");
    
    renderer.Shutdown();
    vrSystem.Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
    
    LOG("VR Paint terminated successfully");
    return 0;
}
