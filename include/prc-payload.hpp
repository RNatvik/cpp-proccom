#ifndef PROCCOM_PAYLOAD_HPP
#define PROCCOM_PAYLOAD_HPP

#include <prc-common.hpp>
#include <prc-util.hpp>

namespace prc {
    struct Payload {
    public:
        virtual void toBytes(std::vector<uint8_t>& bytes) = 0;
        virtual bool fromBytes(std::vector<uint8_t>& bytes) = 0;

    };

}  // namespace prc

#endif