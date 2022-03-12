#ifndef PROCCOM_PUBLISHER_HPP
#define PROCCOM_PUBLISHER_HPP

#include <prc-common.hpp>

namespace prc {
    class Publisher {
    private:
        NodeType type;
        std::string id;
        uint64_t heartbeatTimeout;
        std::vector<std::string> topics;

        bool running;
        soc::UdpSocket socket;
        soc::TSQueue<std::vector<uint8_t>> rxQueue;
        Event newMessage;
        std::thread runThread;
        std::thread admThread;

    public:
        Publisher(std::string id, std::string ip, int port, uint64_t heartbeatTimeout = 20000) :
            socket(
                [this](std::vector<uint8_t>& bytes, size_t length, std::string ip, int port) {this->inboundHandler(bytes, length, ip, port);},
                ip,
                port
            )
        {
            this->type = NodeType::PUBLISHER;
            this->id = id;
            this->running = false;
            this->heartbeatTimeout = heartbeatTimeout;
        }

        bool addTopic(std::string topic) {
            if (!this->running) this->topics.push_back(topic);
            return !this->running;
        }

        void start(std::string brokerIp, uint32_t brokerPort) {
            if (!this->running) {
                this->running = true;
                this->socket.start();
                this->admThread = std::thread([this]() {this->admTask();});
                this->runThread = std::thread([this]() {this->runTask();});

                RegisterMessage msg;
                msg.id = this->id;
                msg.ip = this->socket.getIP();
                msg.port = this->socket.getPort();
                msg.topics = this->topics;
                msg.timestamp = prc::timestamp();
                this->socket.send(brokerIp, brokerPort, msg.toBytes());
            }
        }

        void stop() {
            if (this->running) {
                this->running = false;
                if (this->admThread.joinable()) this->admThread.join();
                if (this->runThread.joinable()) this->runThread.join();
                socket.stop();
            }
        }

    private:
        void inboundHandler(std::vector<uint8_t>& bytes, size_t length, std::string ip, int port) {
            this->rxQueue.push_back(std::vector<uint8_t>(bytes.begin(), bytes.begin() + length));
            this->newMessage.set();
        }

        void admTask() {
            while (this->running) {
                // do something
            }
        }

        void runTask() {
            while (this->running) {
                if (this->rxQueue.empty()) this->newMessage.wait();
                this->newMessage.clear();
                std::vector<uint8_t> bytes = this->rxQueue.pop_front();
                Message msg(bytes);

                switch (msg.msgType) {
                case MessageType::HEARTBEAT:
                    break;
                case MessageType::REGISTER:
                    break;
                }
            }
        }



    };
}

#endif
