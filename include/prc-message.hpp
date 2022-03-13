#ifndef PROCCOM_MESSAGE_HPP
#define PROCCOM_MESSAGE_HPP

#include <prc-common.hpp>
#include <prc-util.hpp>
#include <prc-payload.hpp>


namespace prc {

    enum class MessageType : uint8_t {
        REGISTER = 1,
        UNREGISTER = 2,
        HEARTBEAT = 3,
        PUBLISH = 4
    };

    struct Message {
        MessageType msgType;
        std::string id;
        uint64_t timestamp;

        Message();
        Message(MessageType type);
        Message(std::vector<uint8_t>& bytes);
        ~Message();

        std::vector<uint8_t> toBytes();

        bool fromBytes(std::vector<uint8_t>& bytes);

    protected:
        virtual void _toBytes(std::vector<uint8_t>& bytes);
        virtual bool _fromBytes(std::vector<uint8_t>& bytes, int& offset);
    };

    struct RegisterMessage : public Message {
        NodeType nodeType;
        std::string ip;
        uint32_t port;
        std::vector<std::string> topics;

        RegisterMessage();
        RegisterMessage(std::vector<uint8_t>& bytes);
        ~RegisterMessage();

    protected:
        void _toBytes(std::vector<uint8_t>& bytes);
        bool _fromBytes(std::vector<uint8_t>& bytes, int& offset);
    };

    struct PublishMessage : public Message {
    public:
        std::string topic;
    private:
        std::vector<uint8_t> payload;
    public:
        PublishMessage();
        PublishMessage(std::vector<uint8_t>& bytes);
        ~PublishMessage();

        void attachPayload(Payload& pld);
        void detachPayload(Payload& pld);
    protected:
        void _toBytes(std::vector<uint8_t>& bytes);
        bool _fromBytes(std::vector<uint8_t>& bytes, int& offset);
    };

    struct HeartbeatMessage : public Message {
        HeartbeatMessage();
        HeartbeatMessage(std::vector<uint8_t>& bytes);
        ~HeartbeatMessage();
    };

    struct UnregisterMessage : public Message {
        UnregisterMessage();
        UnregisterMessage(std::vector<uint8_t>& bytes);
        ~UnregisterMessage();
    };

}  // namespace prc

#endif