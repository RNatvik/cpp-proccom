#ifndef PROCCOM_BROKER_HPP
#define PROCCOM_BROKER_HPP

#include <prc-common.hpp>
#include <prc-message.hpp>
#include <prc-node.hpp>

namespace prc {
    class Broker {
    private:

        void handleRegister(std::vector<uint8_t>& bytes) {}
        void sendLinkMessage() {}
        void optionalPappirmoelle() {}

    public:
        Broker(std::string ip, uint32_t port) {}
        ~Broker() {}
    };
}  // namespace prc

#endif