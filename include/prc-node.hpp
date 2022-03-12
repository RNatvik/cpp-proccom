#ifndef PROCCOM_NODE_HPP
#define PROCCOM_NODE_HPP

#include <prc-common.hpp>
#include <UdpSocket.hpp>
#include <map>


namespace prc {

    

    /*------------------------------------------------------------------------------
    Node
    ------------------------------------------------------------------------------*/
    class Node {
    protected:
        bool shutdown;
        bool newMessage;
        uint64_t heartbeatTimeout;
        std::string id;
        soc::UdpSocket socket;
        soc::TSQueue<std::vector<uint8_t>> rxQueue;
        std::thread runThread;
        std::thread admThread;



        void inboundHandler(std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {
            rxQueue.push_back(std::vector<uint8_t>(buffer.begin(), buffer.begin() + length));
            newMessage = true;
        }

        void checkHeartbeats() {
            auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); // chronos voodoo

        }

        void sendHeartbeats() {
            auto currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); // chronos voodoo
        }

        void handleMessage() {
            std::vector<uint8_t> bytes = rxQueue.pop_front();
            Message message(bytes);
            switch (message.msgType) {
            case MessageType::REGISTER:
                this->handleRegister(bytes);
                break;
            case MessageType::LINK:
                //                this->handleLink(om);
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

        void handleHeartbeat(std::vector<uint8_t>& bytes) {}

        virtual void handleRegister(std::vector<uint8_t>& bytes) {} // broker and publisher
//        virtual void handleLink(OwnedMessage& om) {} // publisher specific
        virtual void handlePublish(std::vector<uint8_t>& bytes) {} // subscriber specific
        virtual void optionalPappirmoelle() {}
        virtual void doRun() = 0;

        void run() {
            while (!this->shutdown) {
                if (newMessage) {
                    newMessage = false;
                    doRun();
                }
            }
        }

        void pappirmoelle() {
            using namespace std::chrono_literals;
            while (!this->shutdown) {
                std::this_thread::sleep_for(5000ms);
                this->checkHeartbeats();
                this->sendHeartbeats();

                optionalPappirmoelle();
            }

        }

    public:
        Node(NodeType type, std::string ip, uint32_t port) :
            socket(
                [this](std::vector<uint8_t>& buffer, std::size_t length, std::string ip, int port) {this->inboundHandler(buffer, length, ip, port);},
                ip,
                port
            )
        {
            newMessage = false;
            shutdown = false;
            heartbeatTimeout = 30000;
        }
        ~Node() { this->stop(); }

        void start() {
            this->shutdown = false;
            this->socket.start();
            this->runThread = std::thread([this]() {this->run();});
            this->admThread = std::thread([this]() {this->pappirmoelle();});
        }

        void stop() {
            this->shutdown = true;
            if (this->runThread.joinable()) this->runThread.join();
            if (this->admThread.joinable()) this->admThread.join();
            this->socket.stop();
        }
    };

}  // namespace prc

#endif