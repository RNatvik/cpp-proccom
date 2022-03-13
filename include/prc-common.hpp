#ifndef PROCCOM_COMMON_HPP
#define PROCCOM_COMMON_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional>
#include <condition_variable>
#include <iostream>



namespace prc {



    /*------------------------------------------------------------------------------
    Node Type
    ------------------------------------------------------------------------------*/
    enum class NodeType : uint8_t { BROKER = 1, PUBLISHER = 2, SUBSCRIBER = 3 };

    /*------------------------------------------------------------------------------
    Event flag
    ------------------------------------------------------------------------------*/
    class Event {
    private:
        bool flag;
        std::mutex mutex;
        std::condition_variable cv;

    public:
        Event() { flag = false; }
        ~Event() {}

        void set() {
            std::lock_guard g(mutex);
            flag = true;
            cv.notify_all();
        }

        void clear() {
            std::lock_guard g(mutex);
            flag = false;
        }

        void wait() {
            std::unique_lock lock(mutex);
            cv.wait(lock, [this]() { return flag; });
        }

    };

    /*------------------------------------------------------------------------------
    Lookup table
    ------------------------------------------------------------------------------*/
    struct NodeInfo {
        std::string id;
        NodeType type;
        std::string ip;
        uint32_t port;
        int64_t heartbeat;
        std::vector<std::string> topics;

        NodeInfo() {}
        NodeInfo(const NodeInfo& other) {
            this->id = other.id;
            this->type = other.type;
            this->ip = other.ip;
            this->port = other.port;
            this->heartbeat = other.heartbeat;
            this->topics = other.topics;
        }
        ~NodeInfo() {}
    };

    class NodeLookup {
    private:
        std::map<std::string, NodeInfo*> nodes;
        std::mutex mutex;

    public:
        NodeLookup() {}
        ~NodeLookup() {
            std::scoped_lock lock(mutex);
            for (auto idNodePair : nodes) delete idNodePair.second;
        }

        bool addNode(const NodeInfo& node) {
            std::scoped_lock lock(mutex);
            std::cout << "Lookup:\tAttempting to add " << node.id << std::endl;
            std::cout << "Lookup:\tKeycount: " << this->nodes.count(node.id) << std::endl;
            if (nodes.count(node.id) == 0) {
                NodeInfo* newNode = new NodeInfo(node);
                this->nodes[newNode->id] = newNode;

                return true;
            }
            return false;
        }

        bool removeNode(std::string id) {
            std::scoped_lock lock(mutex);
            std::cout << "Lookup:\tAttempting to remove " << id << std::endl;
            std::cout << "Lookup:\tKeycount: " << this->nodes.count(id) << std::endl;
            if (this->nodes.count(id) > 0) {
                std::cout << "Lookup:\tRemoving " << id << std::endl;
                NodeInfo* ptr = nodes[id];
                nodes.erase(id);
                std::cout << "Lookup:\tKeycount: " << this->nodes.count(id) << std::endl;
                delete ptr;
                return true;
            }
            return false;
        }

        std::vector<NodeInfo*> getNodes() {
            std::scoped_lock lock(mutex);
            std::vector<NodeInfo*> vect;
            for (auto p : this->nodes) {
                vect.push_back(p.second);
            }
            return vect;
        }

        std::vector<NodeInfo*> getPublishers() {
            std::scoped_lock lock(mutex);
            std::vector<NodeInfo*> vect;
            for (auto p : this->nodes) {
                if (p.second->type == NodeType::PUBLISHER) vect.push_back(p.second);
            }
            return vect;
        }

        std::vector<NodeInfo*> getSubscribers() {
            std::scoped_lock lock(mutex);
            std::vector<NodeInfo*> vect;
            for (auto p : this->nodes) {
                if (p.second->type == NodeType::SUBSCRIBER) vect.push_back(p.second);
            }
            return vect;
        }

        bool getBroker(NodeInfo*& ptr) {
            std::scoped_lock lock(mutex);
            for (auto p : this->nodes) {
                if (p.second->type == NodeType::BROKER) {
                    ptr = p.second;
                    return true; // Bad practice?
                }
            }
            return false;
        }

        std::vector<NodeInfo*> getSubscribersByTopic(std::string topic) {
            std::scoped_lock lock(mutex);
            std::vector<NodeInfo*> subscribers = this->getSubscribers();
            return this->getFromTopic(topic, subscribers);
        }

        std::vector<NodeInfo*> getPublishersByTopic(std::string topic) {
            std::scoped_lock lock(mutex);
            std::vector<NodeInfo*> publishers = this->getPublishers();
            return this->getFromTopic(topic, publishers);
        }

        bool getNodeByID(std::string id, NodeInfo*& ptr) {
            std::scoped_lock lock(mutex);
            if (this->nodes.count(id) > 0) {
                ptr = this->nodes[id];
                return true;
            }
            return false;
        }

    private:
        std::vector<NodeInfo*> getFromTopic(std::string topic, std::vector<NodeInfo*>& nodes) {
            std::scoped_lock lock(mutex);
            std::vector<NodeInfo*> vect;
            for (NodeInfo* node : nodes) {
                for (std::string nodeTopic : node->topics) {
                    if (nodeTopic == topic) {
                        vect.push_back(node);
                        break;
                    }
                }
            }
            return vect;
        }
    };


    /*------------------------------------------------------------------------------
    Standardized functions
    ------------------------------------------------------------------------------*/
    int64_t timestamp() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); } // chronos voodoo

}  // namespace prc

#endif