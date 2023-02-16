#ifndef PROCCOM_BROKER_HPP
#define PROCCOM_BROKER_HPP

#include <prc-common.hpp>
#include <prc-node.hpp>
#include <prc-message.hpp>
#include <prc-util.hpp>


namespace prc {
    class Broker : public Node {

    public:
        Broker(std::string id, std::string ip, uint32_t port, uint64_t heartbeatTimeout);

    private:
        void impl_start(std::string brokerIp, uint32_t brokerPort);
        void impl_stop();
        void impl_admTask();
        void impl_runTask();
        void impl_handleRegister(RegisterMessage& msg);
        void impl_handleUnregister(UnregisterMessage& msg);
        void impl_handleHeartbeat(HeartbeatMessage& msg);
        void impl_handlePublish(PublishMessage& msg);

        void _notify_dead_subscriber(NodeInfo *info);

    };
}

#endif