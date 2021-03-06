#include "muon.h"

#include "imgui/imgui.h"
#include "spdlog/spdlog.h"
#include "version.h"
#include "rendering/renderer.h"
#include "text/file_buffer.h"

#include <iostream>
#include <memory>
#include <chrono>

namespace muon {

Muon::Muon(int argc, char** argv) {
    // Command line arguments
    args.define('h', "help", "Show help");
    if (args.parse(argc, argv)) { throw std::runtime_error("Failed to parse arguments"); }

    // MOTD
    spdlog::info("- Muon Editor v" MUON_VERSION);

    // Create window
    win = new Window("Muon v" MUON_VERSION, 1280, 720);
    // win = new Window("Muon v" MUON_VERSION, 3840, 2160);
}

int Muon::run() {
    muon::_FileBuffer fileBuffer("../src/text/file_buffer.h");

    rendering::TextRenderer textRenderer;

    while (true) {
        if (win->newFrame()) { break; }

        // static bool demo = false;
        // ImGui::Checkbox("ImGui Demo", &demo);
        // if (demo) {
        //     ImGui::ShowDemoWindow();
        // }

        // textRenderer.putStr("We do a little rendering", -2, -1, {1.0f, 1.0f, 1.0f, 1.0f});

        // check err
        GLenum err;
        while((err = glGetError()) != GL_NO_ERROR) {
            std::string error;
            switch (err) {
                case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
                case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
                case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
                case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
                case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << "[GL]: " << error << std::endl;
        }    
        
        win->render();
    }

    delete win;
    return 0;
}

}
