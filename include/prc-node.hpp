#ifndef PROCCOM_NODE_HPP
#define PROCCOM_NODE_HPP

#include <prc-common.hpp>
#include <prc-message.hpp>
#include <UdpSocket.hpp>
#include <prc-util.hpp>



namespace prc {

    class Node {
    protected:
        NodeType type;
        std::string id;
        uint64_t heartbeatTimeout;

        NodeLookup nodes;

        bool running;
        soc::UdpSocket socket;
        soc::TSQueue<std::vector<uint8_t>> rxQueue;
        Event newMessage;
        std::thread runThread;
        std::thread admThread;

    public:
        Node(NodeType type, std::string id, std::string ip, int port, uint64_t heartbeatTimeout = 20000);
        ~Node();

        bool isRunning();
        void start(std::string brokerIp = "", uint32_t brokerPort = 0);
        void stop();

    protected:
        void inboundHandler(std::vector<uint8_t>& bytes, size_t length, std::string ip, int port);
        void errorHandler(std::error_code ec, std::string ip, int port);

        void admTask();
        void runTask();
        void handleRegister(std::vector<uint8_t>& bytes);
        void handleUnregister(std::vector<uint8_t>& bytes);
        void handleHeartbeat(std::vector<uint8_t>& bytes);
        void handlePublish(std::vector<uint8_t>& bytes);

        virtual void impl_start(std::string brokerIp, uint32_t brokerPort) = 0;
        virtual void impl_stop() = 0;
        virtual void impl_admTask() = 0;
        virtual void impl_runTask() = 0;
        virtual void impl_handleRegister(RegisterMessage& msg) = 0;
        virtual void impl_handleUnregister(UnregisterMessage& msg) = 0;
        virtual void impl_handleHeartbeat(HeartbeatMessage& msg) = 0;
        virtual void impl_handlePublish(PublishMessage& msg) = 0;

    };

}

#endif