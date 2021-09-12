#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include "wl-roots-includes.h"
#include "server.h"
#define PROJECT_NAME "swipe"


int main(int argc, char **argv) {
    wlr_log_init(WLR_DEBUG, nullptr);
    auto server = Server();
    try {
        server.run();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
