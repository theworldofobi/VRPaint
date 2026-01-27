cmake_minimum_required(VERSION 3.15)
project(VRPaint VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

find_package(OpenGL REQUIRED)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/lib/openvr/headers
    ${CMAKE_SOURCE_DIR}/lib/glm
    ${CMAKE_SOURCE_DIR}/lib/glad/include
    ${CMAKE_SOURCE_DIR}/lib/glfw/include
)

set(SOURCES
    src/main.cpp
    src/VRSystem.cpp
    src/Renderer.cpp
    src/Canvas.cpp
    src/BrushEngine.cpp
    src/InputManager.cpp
    src/ShaderManager.cpp
    lib/glad/src/glad.c
)

set(HEADERS
    include/VRSystem.hpp
    include/Renderer.hpp
    include/Canvas.hpp
    include/BrushEngine.hpp
    include/InputManager.hpp
    include/ShaderManager.hpp
    include/Common.hpp
)

add_executable(VRPaint ${SOURCES} ${HEADERS})

target_link_libraries(VRPaint
    ${OPENGL_LIBRARIES}
    ${CMAKE_SOURCE_DIR}/lib/openvr/lib/linux64/libopenvr_api.so
    ${CMAKE_SOURCE_DIR}/lib/glfw/lib/libglfw3.a
    pthread
    dl
    X11
)

file(COPY ${CMAKE_SOURCE_DIR}/shaders DESTINATION ${CMAKE_BINARY_DIR}/bin)

file(COPY ${CMAKE_SOURCE_DIR}/lib/openvr/bin/linux64/libopenvr_api.so 
     DESTINATION ${CMAKE_BINARY_DIR}/bin)
