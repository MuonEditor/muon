#include "muon.h"
#include "spdlog/spdlog.h"
#include "version.h"
#include "imgui.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include <SDL2/SDL.h>

namespace muon {
    Muon::Muon(int argc, char** argv) {
        // Command line arguments
        args.define('h', "help", "Show help");
        if (args.parse(argc, argv)) { throw std::runtime_error("Failed to parse arguments"); }

        // MOTD
        spdlog::info("- Muon Editor v" MUON_VERSION " -");

        // Create window
        win = new Window("Muon v" MUON_VERSION, 1280, 720);
    }

    int Muon::run() {
        bool demo = false;

        while (true) {
            if (win->newFrame()) { break; }

            ImGui::Checkbox("ImGui Demo", &demo);
            if (demo) {
                ImGui::ShowDemoWindow();
            }
            
            win->render();
        }

        delete win;
        return 0;
    }
}