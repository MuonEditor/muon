#pragma once
#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif
#ifdef __linux__
#include <GL/gl.h>
#endif
#include <string>

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
        SDL_Window* getSDLWindow() { return window; }
        SDL_GLContext getGLContext() { return glContext; }

    private:
        void setupSDL(std::string title);
        void setupImGui();
        void shutdownImGui();
        void shutdownSDL();


        int width, height;
        SDL_Window* window;
        SDL_GLContext glContext;
        const char* glslVersion;

    };
}