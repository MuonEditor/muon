#pragma once
#include "command_args.h"
#include "window.h"

namespace muon {
    class Muon {
    public:
        Muon(int argc, char** argv);

        void setupSDL();
        void setupImGui();

        int run();

    private:
        void handleSDLEvent(SDL_Event event);

        CommandArgsParser args;
        Window* win = NULL;

    };
}
