#ifndef PROCCOM_BROKER_HPP
#define PROCCOM_BROKER_HPP
#include <prc-common.hpp>
#include <prc-node.hpp>
#include <iostream>

namespace prc {
    class Broker : public Node {
    private:


    public:
        Broker(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout = 20000) :
            Node(
                NodeType::BROKER,
                id, ip, port, heartbeatTimeout
            )
        {
        }





    private:
        void impl_start(std::string brokerIp, uint32_t brokerPort) {}

        void impl_stop() {
            std::vector<NodeInfo*> registeredNodes = this->nodes.getNodes();
            UnregisterMessage msg;
            msg.id = this->id;
            msg.timestamp = timestamp();
            for (NodeInfo* node : registeredNodes) {
                this->socket.send(node->ip, node->port, msg.toBytes());
            }
        }

        void impl_admTask() {
            auto currentTimestamp = timestamp();

            // Collect dead publishers
            std::vector<NodeInfo*> publishers = this->nodes.getPublishers();
            std::vector<NodeInfo*> deadPublishers;
            for (NodeInfo* publisher : publishers) {
                if (currentTimestamp > publisher->heartbeat + this->heartbeatTimeout) {
                    deadPublishers.push_back(publisher);
                }
            }
            // Remove dead publishers
            for (NodeInfo* publisher : deadPublishers) {
                this->nodes.removeNode(publisher->id);
            }

            // Collect dead subscribers
            std::vector<NodeInfo*> deadSubscribers;
            std::vector<NodeInfo*> subscribers = this->nodes.getSubscribers();
            for (NodeInfo* subscriber : subscribers) {
                if (currentTimestamp > subscriber->heartbeat + this->heartbeatTimeout) {
                    deadSubscribers.push_back(subscriber);
                }
            }

            // Notify remaining publishers of dead subscribers and remove them
            publishers = this->nodes.getPublishers();
            for (NodeInfo* subscriber : deadSubscribers) {
                UnregisterMessage msg;
                msg.id = subscriber->id;
                msg.timestamp = timestamp();
                std::vector<uint8_t> bytes = msg.toBytes();
                for (std::string topic :subscriber->topics) {
                    for (NodeInfo* publisher : this->nodes.getPublishersByTopic(topic)) {
                        this->socket.send(publisher->ip, publisher->port, bytes);
                    }
                }
            }

            // Send heartbeat to remaining nodes
            HeartbeatMessage msg;
            msg.id = this->id;
            msg.timestamp = timestamp();
            for (NodeInfo* node : this->nodes.getNodes()) {
                this->socket.send(node->ip, node->port, msg.toBytes());
            }
        }

        void impl_runTask() {
        }

        void impl_handleRegister(RegisterMessage& msg) {

            RegisterMessage retMsg;
            retMsg.nodeType = NodeType::BROKER;
            retMsg.id = this->id;
            retMsg.ip = this->socket.getIP();
            retMsg.port = this->socket.getPort();
            retMsg.timestamp = timestamp();
            this->socket.send(msg.ip, msg.port, retMsg.toBytes());
        }

        void impl_handleUnregister(UnregisterMessage& msg) {
            this->nodes.removeNode(msg.id);
        }

        void impl_handleHeartbeat(HeartbeatMessage& msg) {
        }

        void impl_handlePublish(PublishMessage& msg) {}

    };
}

#endif