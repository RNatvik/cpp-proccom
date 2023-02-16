#include <prc-broker.hpp>

namespace prc {

    Broker::Broker(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout = 20000) :
        Node(
            NodeType::BROKER,
            id, ip, port, heartbeatTimeout
        )
    {
    }

    void Broker::impl_start(std::string brokerIp, uint32_t brokerPort) {}

    void Broker::impl_stop() {
        std::vector<NodeInfo*> registeredNodes = this->nodes.getNodes();
        UnregisterMessage msg;
        msg.id = this->id;
        msg.timestamp = timestamp();
        for (NodeInfo* node : registeredNodes) {
            this->socket.send(node->ip, node->port, msg.toBytes());
        }
    }

    void Broker::impl_admTask() {
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
            this->_notify_dead_subscriber(subscriber);
            this->nodes.removeNode(subscriber->id);
        }

        // Send heartbeat to remaining nodes
        HeartbeatMessage msg;
        msg.id = this->id;
        msg.timestamp = timestamp();
        for (NodeInfo* node : this->nodes.getNodes()) {
            this->socket.send(node->ip, node->port, msg.toBytes());
        }
    }

    void Broker::impl_runTask() {}

    void Broker::impl_handleRegister(RegisterMessage& msg) {
        // Send broker information in return to the registered node
        RegisterMessage retMsg;
        retMsg.nodeType = NodeType::BROKER;
        retMsg.id = this->id;
        retMsg.ip = this->socket.getIP();
        retMsg.port = this->socket.getPort();
        retMsg.timestamp = timestamp();
        this->socket.send(msg.ip, msg.port, retMsg.toBytes());

        // If the newly registered node is a subscriber, send its information to relevant publishers
        if (msg.nodeType == NodeType::SUBSCRIBER) {
            auto msgBytes = msg.toBytes();
            for (std::string topic : msg.topics) {
                for (NodeInfo* publisher : this->nodes.getPublishersByTopic(topic)) {
                    this->socket.send(publisher->ip, publisher->port, msgBytes);
                    std::cout << topic << std::endl;
                }
            }
        }

        // If the newly registered node is a publisher, return information about relevant subscribers
        RegisterMessage subMsg;
        subMsg.nodeType = NodeType::SUBSCRIBER;
        if (msg.nodeType == NodeType::PUBLISHER) {
            for (std::string topic : msg.topics) {
                for (NodeInfo* subscriber : this->nodes.getSubscribersByTopic(topic)) {
                    subMsg.id = subscriber->id;
                    subMsg.ip = subscriber->ip;
                    subMsg.port = subscriber->port;
                    subMsg.timestamp = timestamp();
                    subMsg.topics = subscriber->topics;
                    this->socket.send(msg.ip, msg.port, subMsg.toBytes());
                    std::cout << topic << std::endl;
                }
            }
        }
    }

    void Broker::impl_handleUnregister(UnregisterMessage& msg) {
        NodeInfo *node;
        this->nodes.getNodeByID(msg.id, node);
        if (node->type == NodeType::SUBSCRIBER) {this->_notify_dead_subscriber(node);}
    }

    void Broker::impl_handleHeartbeat(HeartbeatMessage& msg) {
    }

    void Broker::impl_handlePublish(PublishMessage& msg) {}


    void Broker::_notify_dead_subscriber(NodeInfo *info) {
        UnregisterMessage msg;
        msg.id = info->id;
        msg.timestamp = timestamp();
        std::vector<uint8_t> bytes = msg.toBytes();
        for (std::string topic : info->topics) {
            for (NodeInfo* publisher : this->nodes.getPublishersByTopic(topic)) {
                this->socket.send(publisher->ip, publisher->port, bytes);
            }
        }
    }

}