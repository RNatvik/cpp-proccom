#ifndef PROCCOM_SUBSCRIBER_HPP
#define PROCCOM_SUBSCRIBER_HPP

#include <prc-common.hpp>
#include <prc-node.hpp>
#include <prc-message.hpp>
#include <prc-util.hpp>


namespace prc {
    class Subscriber : public Node {
    private:
        std::vector<std::string> topics;
        std::function<void(PublishMessage &msg)> publishHandler;

    public:
        Subscriber(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout=20000);

        bool addTopic(std::string topic);
        void attachPublishHandler(std::function<void(PublishMessage &msg)> publishHandler);

    private:
        void impl_start(std::string brokerIp, uint32_t brokerPort);
        void impl_stop();
        void impl_admTask();
        void impl_runTask();
        void impl_handleRegister(RegisterMessage& msg);
        void impl_handleUnregister(UnregisterMessage& msg);
        void impl_handleHeartbeat(HeartbeatMessage& msg);
        void impl_handlePublish(PublishMessage& msg);
    };
}

#endif
