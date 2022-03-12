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

        NodeLookup nodes;

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

        void publish(std::string topic, Payload& pld) {
            PublishMessage msg;
            msg.id = this->id;
            msg.topic = topic;
            msg.timestamp = timestamp();
            msg.attachPayload(pld);
            std::vector<uint8_t> bytes = msg.toBytes();

            for (auto subscriber : this->nodes.getSubscribersByTopic(topic)) {
                this->socket.send(subscriber->ip, subscriber->port, bytes);
            }

        }

        bool isRunning() { return this->running; }

        void start(std::string brokerIp, uint32_t brokerPort) {
            if (!this->running) {
                this->running = true;
                this->socket.start();
                this->admThread = std::thread([this]() {this->admTask();});
                this->runThread = std::thread([this]() {this->runTask();});

                RegisterMessage msg;
                msg.nodeType = NodeType::PUBLISHER;
                msg.id = this->id;
                msg.ip = this->socket.getIP();
                msg.port = this->socket.getPort();
                msg.topics = this->topics;
                msg.timestamp = timestamp();
                this->socket.send(brokerIp, brokerPort, msg.toBytes());
            }
        }

        void stop() {
            if (this->running) {
                socket.stop();
                this->running = false;
                if (this->admThread.joinable()) this->admThread.join();
                if (this->runThread.joinable()) this->runThread.join();
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
                using namespace std::chrono_literals;
                std::this_thread::sleep_for(5000ms);
                
                auto currentTimestamp = timestamp();
                NodeInfo* brokerInfo;
                bool brokerRegistered = this->nodes.getBroker(brokerInfo);
                bool brokerDisconnected = false;
                if (brokerRegistered) brokerDisconnected = currentTimestamp > (brokerInfo->heartbeat + this->heartbeatTimeout);
                if (brokerRegistered && !brokerDisconnected) {
                    HeartbeatMessage msg;
                    msg.id = this->id;
                    msg.timestamp = currentTimestamp;
                    this->socket.send(brokerInfo->ip, brokerInfo->port, msg.toBytes());
                } else {
                    //What now?
                }
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
                    this->handleHeartbeat(bytes);
                    break;
                case MessageType::REGISTER:
                    this->handleRegister(bytes);
                    break;
                default:
                    break;
                }
            }
        }

        void handleRegister(std::vector<uint8_t>& bytes) {
            RegisterMessage msg(bytes);
            NodeInfo info;
            info.type = msg.nodeType;
            info.id = msg.id,
            info.ip = msg.ip;
            info.port = msg.port;
            info.heartbeat = msg.timestamp;
            info.topics = msg.topics;
            this->nodes.addNode(info);
        }

        void handleHeartbeat(std::vector<uint8_t>& bytes) {
            HeartbeatMessage msg(bytes);
            NodeInfo* ptr;
            bool success = this->nodes.getNodeByID(msg.id, ptr);
            if (success) {
                ptr->heartbeat = msg.timestamp;
            }
        }



    };
}

#endif
