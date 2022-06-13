#include "muon.h"
#include <exception>
#include "spdlog/spdlog.h"

int main(int argc, char** argv) {
    try {
        muon::Muon muon(argc, argv);
        return muon.run();
    }
    catch (std::exception e) {
        spdlog::error(e.what());
        return -1;
    }
}
