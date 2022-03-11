#ifndef PROCCOM_NODE_HPP
#define PROCCOM_NODE_HPP

#include <prc-common.hpp>
#include <prc-socket.hpp>
#include <prc-queue.hpp>

namespace prc {


    /*------------------------------------------------------------------------------
    Endpoint
    ------------------------------------------------------------------------------*/
    struct Endpoint {
        std::string ip;
        uint32_t port;

        Endpoint() {}
        Endpoint(std::string ip, uint32_t port) {
            this->ip = ip;
            this->port = port;
        }
        ~Endpoint() {}

        bool operator==(const Endpoint& other) {
            return this->ip == other.ip && this->port == other.port;
        }
    };

    /*------------------------------------------------------------------------------
    Node
    ------------------------------------------------------------------------------*/
    class Node {
    protected:
        bool shutdown;
        UdpSocket socket;
        TSQueue<std::vector<uint8_t>> rxQueue;
        bool newMessage;

        void inboundHandler(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {
            std::vector<uint8_t> bytes(buffer.begin(), buffer.begin() + length);
            rxQueue.push_back(bytes);
            newMessage = true;
        }

        void handleMessage(std::vector<uint8_t>& bytes) {
            Message message(bytes);
            switch (message.msgType) {
            case MessageType::REGISTER:
                this->handleRegister(bytes);
                break;
            case MessageType::LINK:
                this->handleLink(bytes);
                break;
            case MessageType::HEARTBEAT:
                this->handleHeartbeat(bytes);
                break;
            case MessageType::PUBLISH:
                this->handlePublish(bytes);
                break;
            default:
                break;
            }
        }

        virtual void handleRegister(std::vector<uint8_t>& bytes) {}
        virtual void handleLink(std::vector<uint8_t>& bytes) {}
        virtual void handleHeartbeat(std::vector<uint8_t>& bytes) {}
        virtual void handlePublish(std::vector<uint8_t>& bytes) {}

        virtual void run() = 0;

    public:
        Node(NodeType type, std::string ip, uint32_t port) :
            socket(
                [this](std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {this->inboundHandler(buffer, length, ip, port);},
                ip,
                port
            )
        {
            newMessage = false;
        }
        ~Node() {}

        void start() {
            this->shutdown = false;
            this->socket.start();
            this->run(); 
        }
        void stop() {
            this->shutdown = true;
            this->socket.stop();
        }
    };

}  // namespace prc

#endif