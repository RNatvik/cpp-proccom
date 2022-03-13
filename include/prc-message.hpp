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

        Message() {}
        Message(MessageType type) { this->msgType = type; }
        Message(std::vector<uint8_t>& bytes) { this->fromBytes(bytes); }
        ~Message() {}

        std::vector<uint8_t> toBytes() {
            std::vector<uint8_t> bytes;
            prc::varToBytes(msgType, bytes);
            prc::stringToBytes(id, bytes);
            prc::varToBytes(timestamp, bytes);
            _toBytes(bytes);  // Subclass deconstruct
            return bytes;
        }

        bool fromBytes(std::vector<uint8_t>& bytes) {
            int offset = 0;
            prc::varFromBytes(msgType, bytes, offset);
            prc::stringFromBytes(id, bytes, offset);
            prc::varFromBytes(timestamp, bytes, offset);
            bool success = _fromBytes(bytes, offset);  // Subclass reconstruct
            return success;
        }

    protected:
        virtual void _toBytes(std::vector<uint8_t>& bytes) {}
        virtual bool _fromBytes(std::vector<uint8_t>& bytes, int& offset) {
            return true;
        }
    };

    struct RegisterMessage : public Message {
        NodeType nodeType;
        std::string ip;
        uint32_t port;
        std::vector<std::string> topics;


        RegisterMessage() : Message(MessageType::REGISTER) {}

        RegisterMessage(std::vector<uint8_t>& bytes) : Message() { this->fromBytes(bytes); }

        ~RegisterMessage() {}

    protected:
        void _toBytes(std::vector<uint8_t>& bytes) override {
            prc::varToBytes(nodeType, bytes);
            prc::stringToBytes(ip, bytes);
            prc::varToBytes(port, bytes);
            for (std::string topic : topics) {
                prc::stringToBytes(topic, bytes);
            }
            bytes.push_back(topics.size());
        }

        bool _fromBytes(std::vector<uint8_t>& bytes, int& offset) override {
            prc::varFromBytes(nodeType, bytes, offset);
            prc::stringFromBytes(ip, bytes, offset);
            prc::varFromBytes(port, bytes, offset);
            uint8_t numTopics = bytes.back();
            bytes.pop_back();
            for (int i = 0; i < numTopics; i++) {
                std::string str;
                prc::stringFromBytes(str, bytes, offset);
                topics.push_back(str);
            }
            return true;
        }
    };

    struct PublishMessage : public Message {
    public:
        std::string topic;
    private:
        std::vector<uint8_t> payload;
    public:
        PublishMessage() : Message(MessageType::PUBLISH) {}

        PublishMessage(std::vector<uint8_t>& bytes) : Message() { this->fromBytes(bytes); }

        ~PublishMessage() {}

        void attachPayload(Payload& pld) {
            payload.clear();
            pld.toBytes(payload);
        }

        void detachPayload(Payload& pld) { pld.fromBytes(payload); }

    protected:
        void _toBytes(std::vector<uint8_t>& bytes) {
            prc::stringToBytes(topic, bytes);
            for (int i = 0; i < payload.size(); i++) {
                bytes.push_back(payload[i]);
            }
        }

        bool _fromBytes(std::vector<uint8_t>& bytes, int& offset) {
            prc::stringFromBytes(topic, bytes, offset);
            for (int i = offset; i < bytes.size(); i++) {
                payload.push_back(bytes[i]);
            }
            return true;
        }
    };

    struct HeartbeatMessage : public Message {
        HeartbeatMessage() : Message(MessageType::HEARTBEAT) {}
        HeartbeatMessage(std::vector<uint8_t> bytes) : Message() { this->fromBytes(bytes); }
        ~HeartbeatMessage() {}
    };

    struct UnregisterMessage : public Message {
        UnregisterMessage() : Message(MessageType::HEARTBEAT) {}
        UnregisterMessage(std::vector<uint8_t>& bytes) : Message() { this->fromBytes(bytes); }
        ~UnregisterMessage() {}
    };

}  // namespace prc

#endif