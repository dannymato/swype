#include <exception>
#include <iostream>
#include "wl-roots-includes.h"
#include "server.h"
#include <unistd.h>
#define PROJECT_NAME "swipe"


int main(int argc, char **argv) {
    wlr_log_init(WLR_DEBUG, nullptr);
    try {
        auto server = Server();
        if (fork() == 0) {
            execl("/bin/sh", "/bin/sh", "-c", "konsole", (void*) nullptr);
        }
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
