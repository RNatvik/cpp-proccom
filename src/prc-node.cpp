#include <prc-node.hpp>

namespace prc {


    Node::Node(NodeType type, std::string id, std::string ip, int port, uint64_t heartbeatTimeout) :
        socket(
            [this](std::vector<uint8_t>& bytes, size_t length, std::string ip, int port) { this->inboundHandler(bytes, length, ip, port); },
            ip,
            port
        ) {
        this->type = type;
        this->id = id;
        this->heartbeatTimeout = heartbeatTimeout;
        this->running = false;

        this->socket.attachErrorHandler(
            [this](std::error_code ec, std::string ip, int port) {
                NodeInfo* ptr;
                if (this->nodes.getNodeByEndpoint(ip, port, ptr)) {
                    this->nodes.removeNode(ptr->id);
                }
                return true;
            }
        );
    }
    Node::~Node() {}

    bool Node::isRunning() { return this->running; }

    void Node::start(std::string brokerIp, uint32_t brokerPort) {
        if (!this->running) {
            this->running = true;
            this->socket.start();
            this->admThread = std::thread([this]() {this->admTask();});
            this->runThread = std::thread([this]() {this->runTask();});
            this->impl_start(brokerIp, brokerPort);
        }
    }

    void Node::stop() {
        std::cout << "Stop called" << std::endl;
        if (this->running) {
            this->impl_stop();
            using namespace std::chrono_literals;
            this->running = false;
            std::cout << "Waiting for admThread" << std::endl;
            if (this->admThread.joinable()) this->admThread.join();
            std::cout << "Waiting for runThread" << std::endl;
            this->newMessage.set();
            if (this->runThread.joinable()) this->runThread.join();
            std::cout << "Sleeping 2 sec" << std::endl;
            std::this_thread::sleep_for(2s);
            std::cout << "Stopping socket" << std::endl;
            socket.stop();
            std::cout << "Stop call finished" << std::endl;
        }
    }

    void Node::inboundHandler(std::vector<uint8_t>& bytes, size_t length, std::string ip, int port) {
        this->rxQueue.push_back(std::vector<uint8_t>(bytes.begin(), bytes.begin() + length));
        this->newMessage.set();
    }

    void Node::admTask() {
        while (this->running) {
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(5000ms);
            if (!this->running) break;

            this->impl_admTask();
        }
    }

    void Node::runTask() {
        while (this->running) {
            if (this->rxQueue.empty()) this->newMessage.wait();
            this->newMessage.clear();
            if (!this->running) break;

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

    void Node::handleRegister(std::vector<uint8_t>& bytes) {
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

    void Node::handleUnregister(std::vector<uint8_t>& bytes) {
        UnregisterMessage msg(bytes);
        std::cout << "Unregister received from " << msg.id << std::endl;
        this->impl_handleUnregister(msg);
    }

    void Node::handleHeartbeat(std::vector<uint8_t>& bytes) {
        HeartbeatMessage msg(bytes);
        NodeInfo* ptr;
        bool nodeRegistered = this->nodes.getNodeByID(msg.id, ptr);
        if (nodeRegistered) {
            ptr->heartbeat = msg.timestamp;
            this->impl_handleHeartbeat(msg);
        }
    }

    void Node::handlePublish(std::vector<uint8_t>& bytes) {
        PublishMessage msg(bytes);
        this->impl_handlePublish(msg);
    }



}  // namespace prc