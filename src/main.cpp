#include "AsyncServer.hpp"

int main() {
    try {
        uint16_t port = 8080;
        AsyncServer server(port);
        server.start();
        server.join();
    } catch (std::exception& e) {
        std::printf("Exception: %s\n", e.what());
    }
}
