#include <cassert>
#include <exception>
#include <iostream>
#include <memory>
#include <functional>
#include <vector>
#include "wl-roots-includes.h"
#include "server.h"
#include <unistd.h>
#define PROJECT_NAME "swipe"


int main(int argc, char **argv) {
    wlr_log_init(WLR_DEBUG, nullptr);
    auto server = Server();
    try {
        if (fork() == 0) {
            execl("/bin/sh", "/bin/sh", "-c", "konsole", (void*)NULL);
        }
        server.run();

    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
