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
        NodeType type;
        std::string id;
        uint64_t heartbeatTimeout;

        NodeLookup nodes;

        bool running;
        soc::UdpSocket socket;
        soc::TSQueue<std::vector<uint8_t>> rxQueue;
        Event newMessage;
        std::thread runThread;
        std::thread admThread;

    public:
        Node(NodeType type, std::string id, std::string ip, int port, uint64_t heartbeatTimeout = 20000) :
            socket(
                [this](std::vector<uint8_t>& bytes, size_t length, std::string ip, int port) { this->inboundHandler(bytes, length, ip, port); },
                ip,
                port
            ) {
            this->type = type;
            this->id = id;
            this->heartbeatTimeout = heartbeatTimeout;
            this->running = false;
        }
        ~Node() {}

        bool isRunning() { return this->running; }

        void start(std::string brokerIp="", uint32_t brokerPort=-1) {
            if (!this->running) {
                this->running = true;
                this->socket.start();
                this->admThread = std::thread([this]() {this->admTask();});
                this->runThread = std::thread([this]() {this->runTask();});
                this->impl_start(brokerIp, brokerPort);
            }
        }

        void stop() {
            if (this->running) {
                this->impl_stop();
                socket.stop();
                this->running = false;
                if (this->admThread.joinable()) this->admThread.join();
                if (this->runThread.joinable()) this->runThread.join();
            }
        }

    protected:
        void inboundHandler(std::vector<uint8_t>& bytes, size_t length, std::string ip, int port) {
            this->rxQueue.push_back(std::vector<uint8_t>(bytes.begin(), bytes.begin() + length));
            this->newMessage.set();
        }
        void admTask() {
            while (this->running) {
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(5000ms);
                this->impl_admTask();
            }
        }
        void runTask() {
            while (this->running) {
                if (this->rxQueue.empty()) this->newMessage.wait();
                this->newMessage.clear();

                std::vector<uint8_t> bytes = this->rxQueue.pop_front();
                Message msg(bytes);
                switch (msg.msgType) {

                case MessageType::REGISTER:
                    this->handleRegister(bytes);
                    break;
                case MessageType::UNREGISTER:
                    this->handleUnregister(bytes);
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
                this->impl_runTask();
            }
        }

        void handleRegister(std::vector<uint8_t>& bytes) {
            RegisterMessage msg(bytes);
            NodeInfo info;
            info.type = msg.nodeType;
            info.id = msg.id;
            info.ip = msg.ip;
            info.port = msg.port;
            info.heartbeat = msg.timestamp;
            info.topics = msg.topics;
            bool success = this->nodes.addNode(info);
            if (success) this->impl_handleRegister(msg);
        }

        void handleUnregister(std::vector<uint8_t>& bytes) {
            UnregisterMessage msg(bytes);
            this->impl_handleUnregister(msg);
        }

        void handleHeartbeat(std::vector<uint8_t>& bytes) {
            HeartbeatMessage msg(bytes);
            NodeInfo* ptr;
            bool nodeRegistered = this->nodes.getNodeByID(msg.id, ptr);
            if (nodeRegistered) {
                ptr->heartbeat = msg.timestamp;
                this->impl_handleHeartbeat(msg);
            }
        }

        void handlePublish(std::vector<uint8_t>& bytes) {
            PublishMessage msg(bytes);
            this->impl_handlePublish(msg);
        }


        virtual void impl_start(std::string brokerIp, uint32_t brokerPort) = 0;
        virtual void impl_stop() = 0;
        virtual void impl_admTask() = 0;
        virtual void impl_runTask() = 0;
        virtual void impl_handleRegister(RegisterMessage& msg) = 0;
        virtual void impl_handleUnregister(UnregisterMessage& msg) = 0;
        virtual void impl_handleHeartbeat(HeartbeatMessage& msg) = 0;
        virtual void impl_handlePublish(PublishMessage& msg) = 0;

    };

}  // namespace prc

#endif