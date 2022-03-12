#ifndef PROCCOM_PUBLISHER_HPP
#define PROCCOM_PUBLISHER_HPP

#include <prc-common.hpp>
#include <prc-node.hpp>

namespace prc {
    class Publisher : Node {
    private:
        std::vector<std::string> topics;


    public:
        Publisher(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout = 20000) :
            Node(
                NodeType::PUBLISHER,
                id, ip, port, heartbeatTimeout
            )
        {
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

    private:
        void impl_start(std::string brokerIp, uint32_t brokerPort) {
            RegisterMessage msg;
            msg.nodeType = NodeType::PUBLISHER;
            msg.id = this->id;
            msg.ip = this->socket.getIP();
            msg.port = this->socket.getPort();
            msg.topics = this->topics;
            msg.timestamp = timestamp();
            this->socket.send(brokerIp, brokerPort, msg.toBytes());
        }

        void impl_stop() {
            UnregisterMessage msg;
            msg.id = this->id;
            msg.timestamp = timestamp();
            NodeInfo* brokerInfo;
            bool brokerExists = this->nodes.getBroker(brokerInfo);
            if (brokerExists) this->socket.send(brokerInfo->ip, brokerInfo->port, msg.toBytes());
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

        void impl_runTask() {}

        void impl_handleRegister(RegisterMessage& msg) {}

        void impl_handleUnregister(UnregisterMessage& msg) {
            this->nodes.removeNode(msg.id);
        }

        void impl_handleHeartbeat(HeartbeatMessage& msg) {}

        void impl_handlePublish(PublishMessage& msg) {}

    };
}

#endif
