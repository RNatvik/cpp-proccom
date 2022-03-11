#ifndef PROCCOM_BROKER_HPP
#define PROCCOM_BROKER_HPP

#include <prc-common.hpp>
#include <prc-message.hpp>
#include <prc-node.hpp>

namespace prc {
class Broker : public Node {
   private:
    std::vector<Endpoint *> nodes;
    std::map<std::string, std::vector<Endpoint *>> publishedTopics;
    std::map<std::string, std::vector<Endpoint *>> subscribedTopics;
    bool shutdown;

    void run() {
        while (!this->shutdown) {
            
        }
    }

    void handleRegister(std::vector<uint8_t> &bytes) {
        RegisterMessage message(bytes);

        Endpoint *target = new Endpoint(message.ip, message.port);
        bool targetExists = false;
        for (Endpoint *node : nodes) {
            targetExists |= *node == *target;
            if (targetExists) {
                delete target;
                target = node;
                return; // Already registered, do nothing
            }
        }
        nodes.push_back(target);
        if (message.nodeType == NodeType::SUBSCRIBER) {
            for (std::string topic : message.topics) {
                if (subscribedTopics.count(topic) > 0) {
                    subscribedTopics[topic].push_back(target);
                } else {
                    std::vector<Endpoint *> vect;
                    vect.push_back(target);
                    subscribedTopics[topic] = vect;
                }
                if (publishedTopics.count(topic) > 0) {
                    auto publishers = publishedTopics[topic];
                    for (Endpoint *publisher : publishers) {
                        sendLinkMessage(publisher, target, topic);
                    }
                }
            }
        } else if (message.nodeType == NodeType::PUBLISHER) {
            for (std::string topic : message.topics) {
                if (publishedTopics.count(topic) > 0) {
                    publishedTopics[topic].push_back(target);
                } else {
                    std::vector<Endpoint *> vect;
                    vect.push_back(target);
                    publishedTopics[topic] = vect;
                }
            }
        }
    }
    void handleHeartbeat(std::vector<uint8_t> &bytes) {}
    void sendLinkMessage(Endpoint *publisher, Endpoint *subscriber, std::string topic) {}

   public:
    Broker(std::string ip, uint32_t port): Node(NodeType::BROKER, ip, port) {}
    ~Broker() {}
};
}  // namespace prc

#endif