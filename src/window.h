#pragma once
#include <string>
#include <GLFW/glfw3.h>

// Include OpenGL
#if defined(_WIN32)
#include <windows.h>
#include <GL/gl.h>
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#elif defined(__ANDROID__)
#include <EGL/egl.h>
#include <GLES3/gl3.h>
#else
#include <GL/gl.h>
#endif

#define WINDOW_FLAGS ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoDecoration

namespace muon {
    class Window {
    public:
        Window(std::string title, int width, int height);
        ~Window();

        bool newFrame();
        void render();

        void setTitle(std::string title);
        void setSize(int w, int h);
        void setPos(int x, int y);

        void getSize(int& w, int& h);
        void getPos(int& x, int& y);
        GLFWwindow* getGLFWWindow() { return window; }

    private:
        void setupGLFW(std::string title);
        void setupImGui();
        void shutdownImGui();
        void shutdownGLFW();

        static void glfwErrorCallback(int error, const char* description);


        int width, height;
        const char* glslVersion;
        GLFWwindow* window;
    };
}