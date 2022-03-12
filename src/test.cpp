#include <UdpSocket.hpp>
#include <iostream>


class EchoServer {
private:
    soc::UdpSocket socket;

    void ihandle(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {
        std::cout << "\n\nReceived " << length << " bytes:\n";
        for (int i = 0; i < length; i++) {
            std::cout << buffer[i];
        }
        std::vector<uint8_t> tx(buffer.begin(), buffer.begin() + length);
        socket.send(ip, port, tx);
    }

public:

    EchoServer(std::string ip, int port) :
        socket(
            [this](std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {this->ihandle(buffer, length, ip, port);},
            ip,
            port
        )
    {}
    ~EchoServer() { stop(); }
    void start() { socket.start(); }
    void stop() { socket.stop(); }

};



int main() {
    EchoServer server("127.0.0.1", 6969);
    server.start();
    // 2 min delay before server shutdown
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(20000ms);

    server.stop();
    return 0;
}