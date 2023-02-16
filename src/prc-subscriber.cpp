#include <prc-subscriber.hpp>

namespace prc {

        Subscriber::Subscriber(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout) :
            Node(
                NodeType::SUBSCRIBER,
                id, ip, port, heartbeatTimeout
            )
        {
            this->publishHandler = [this](PublishMessage &msg) {};
        }

        bool Subscriber::addTopic(std::string topic) {
            if (!this->running) this->topics.push_back(topic);
            return !this->running;
        }

        void Subscriber::attachPublishHandler(std::function<void(PublishMessage &msg)> publishHandler) {
            this->publishHandler = publishHandler;
        }

        void Subscriber::impl_start(std::string brokerIp, uint32_t brokerPort) {
            RegisterMessage msg;
            msg.nodeType = this->type;
            msg.id = this->id;
            msg.ip = this->socket.getIP();
            msg.port = this->socket.getPort();
            msg.topics = this->topics;
            msg.timestamp = timestamp();
            this->socket.send(brokerIp, brokerPort, msg.toBytes());
        }

        void Subscriber::impl_stop() {
            UnregisterMessage msg;
            msg.id = this->id;
            msg.timestamp = timestamp();
            NodeInfo* brokerInfo;
            bool brokerExists = this->nodes.getBroker(brokerInfo);
            if (brokerExists) this->socket.send(brokerInfo->ip, brokerInfo->port, msg.toBytes());
        }

        void Subscriber::impl_admTask() {
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

        void Subscriber::impl_runTask() {}

        void Subscriber::impl_handleRegister(RegisterMessage& msg) {}

        void Subscriber::impl_handleUnregister(UnregisterMessage& msg) {}

        void Subscriber::impl_handleHeartbeat(HeartbeatMessage& msg) {}

        void Subscriber::impl_handlePublish(PublishMessage& msg) {
            this->publishHandler(msg);
        }


}

