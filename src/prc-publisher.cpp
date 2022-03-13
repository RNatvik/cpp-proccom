#include <prc-publisher.hpp>

namespace prc {

    Publisher::Publisher(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout) :
        Node(
            NodeType::PUBLISHER,
            id, ip, port, heartbeatTimeout
        )
    {
    }

    bool Publisher::addTopic(std::string topic) {
        if (!this->running) this->topics.push_back(topic);
        return !this->running;
    }

    void Publisher::publish(std::string topic, Payload& pld) {
        PublishMessage msg;
        msg.id = this->id;
        msg.topic = topic;
        msg.timestamp = timestamp();
        msg.attachPayload(pld);
        std::vector<uint8_t> bytes = msg.toBytes();

        for (auto subscriber : this->nodes.getSubscribersByTopic(topic)) {
            std::string ip = subscriber->ip;
            uint32_t port = subscriber->port;
            this->socket.send(ip, port, bytes);
        }

    }

    void Publisher::impl_start(std::string brokerIp, uint32_t brokerPort) {
        RegisterMessage msg;
        msg.nodeType = NodeType::PUBLISHER;
        msg.id = this->id;
        msg.ip = this->socket.getIP();
        msg.port = this->socket.getPort();
        msg.topics = this->topics;
        msg.timestamp = timestamp();
        this->socket.send(brokerIp, brokerPort, msg.toBytes());
    }

    void Publisher::impl_stop() {
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

    void Publisher::impl_admTask() {
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

    void Publisher::impl_runTask() {
    }

    void Publisher::impl_handleRegister(RegisterMessage& msg) {

    }

    void Publisher::impl_handleUnregister(UnregisterMessage& msg) {
        this->nodes.removeNode(msg.id);
    }

    void Publisher::impl_handleHeartbeat(HeartbeatMessage& msg) {}

    void Publisher::impl_handlePublish(PublishMessage& msg) {}

}

