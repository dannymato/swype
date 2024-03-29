#include "wl-roots-includes.h"
#include "server.h"

#include <exception>
#include <iostream>
#define PROJECT_NAME "swipe"


int main() {
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
