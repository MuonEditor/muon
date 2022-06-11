#pragma once
#include "command_args.h"
#include "window.h"

namespace muon {
    class Muon {
    public:
        Muon(int argc, char** argv);

        int run();

    private:
        CommandArgsParser args;
        Window* win = NULL;

    };
}
