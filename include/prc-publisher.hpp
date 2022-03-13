#ifndef PROCCOM_PUBLISHER_HPP
#define PROCCOM_PUBLISHER_HPP

#include <prc-common.hpp>
#include <prc-node.hpp>
#include <prc-message.hpp>
#include <prc-util.hpp>


namespace prc {
    class Publisher : public Node {
    private:
        std::vector<std::string> topics;


    public:
        Publisher(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout=20000);

        bool addTopic(std::string topic);
        void publish(std::string topic, Payload& pld);

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
