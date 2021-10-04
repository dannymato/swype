#include <exception>
#include <iostream>
#include "wl-roots-includes.h"
#include "server.h"
#define PROJECT_NAME "swipe"


int main(int argc, char **argv) {
    wlr_log_init(WLR_DEBUG, nullptr);
    try {
        auto server = Server();
        server.run();

    } catch (std::exception& e) {
        std::cerr << "FATAL ERROR: " << e.what() << std::endl;
        return -1;
    } catch (const char* message) {
        std::cerr << "FATAL ERROR: " << message << std::endl;
        return -1;
    }
    return 0;
}
