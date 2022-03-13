#ifndef PROCCOM_SUBSCRIBER_HPP
#define PROCCOM_SUBSCRIBER_HPP

#include <prc-common.hpp>
#include <prc-node.hpp>
#include <prc-message.hpp>

namespace prc {
    class Subscriber : public Node {
    private:
        std::vector<std::string> topics;
        std::function<void(PublishMessage &msg)> publishHandler;

    public:
        Subscriber(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout = 20000) :
            Node(
                NodeType::SUBSCRIBER,
                id, ip, port, heartbeatTimeout
            )
        {
            this->publishHandler = [this](PublishMessage &msg) {};
        }

        bool addTopic(std::string topic) {
            if (!this->running) this->topics.push_back(topic);
            return !this->running;
        }

        void attachPublishHandler(std::function<void(PublishMessage &msg)> publishHandler) {
            this->publishHandler = publishHandler;
        }

    private:
        void impl_start(std::string brokerIp, uint32_t brokerPort) {
            RegisterMessage msg;
            msg.nodeType = this->type;
            msg.id = this->id;
            msg.ip = this->socket.getIP();
            msg.port = this->socket.getPort();
            msg.topics = this->topics;
            msg.timestamp = timestamp();
            this->socket.send(brokerIp, brokerPort, msg.toBytes());
        }

        void impl_stop() {
            std::cout << "impl_stop()" << std::endl;
            UnregisterMessage msg;
            msg.id = this->id;
            msg.timestamp = timestamp();
            NodeInfo* brokerInfo;
            bool brokerExists = this->nodes.getBroker(brokerInfo);
            std::cout << "Broker exists: " << brokerExists << "... sending unregister if true" << std::endl;
            if (brokerExists) this->socket.send(brokerInfo->ip, brokerInfo->port, msg.toBytes());
            std::cout << "end impl_stop()" << std::endl;
        }

        void impl_admTask() {
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
            }
            else {
                //What now?
            }
        }

        void impl_runTask() {
        }

        void impl_handleRegister(RegisterMessage& msg) {

        }

        void impl_handleUnregister(UnregisterMessage& msg) {
            this->nodes.removeNode(msg.id);
        }

        void impl_handleHeartbeat(HeartbeatMessage& msg) {}

        void impl_handlePublish(PublishMessage& msg) {
            std::cout << "Received msg: " << std::endl;
            std::cout << "\tID:\t" << msg.id << std::endl;
            std::cout << "\tTopic:\t" << msg.topic << std::endl;
            std::cout << "\tTimestamp:\t" << msg.timestamp << std::endl;
            this->publishHandler(msg);
        }

    };
}

#endif
