#include <UdpSocket.hpp>
#include <iostream>
#include <prc-common.hpp>
#include <prc-message.hpp>


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

    void ihandleMessage(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {
        std::vector<uint8_t> vect(buffer.begin(), buffer.begin() + length);
        prc::RegisterMessage msg(vect);
        std::cout << "Received:" << std::endl;
        std::cout << "Node id:\t" << msg.id << std::endl;
        std::cout << "Node ip:\t" << msg.ip << std::endl;
        std::cout << "Node port:\t" << msg.port << std::endl;
        std::cout << "Node timestamp:\t" << msg.timestamp << std::endl;

        prc::RegisterMessage retMsg;
        retMsg.nodeType = prc::NodeType::BROKER;
        retMsg.id = "EchoServer";
        retMsg.ip = "127.0.0.1";
        retMsg.port = 6969;
        retMsg.timestamp = prc::timestamp();
        socket.send(ip, port, retMsg.toBytes());
    }

public:

    EchoServer(std::string ip, int port) :
        socket(
            [this](std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {this->ihandleMessage(buffer, length, ip, port);},
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