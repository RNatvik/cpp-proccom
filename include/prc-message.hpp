#ifndef PROCCOM_MESSAGE_HPP
#define PROCCOM_MESSAGE_HPP

#include <prc-common.hpp>
#include <prc-payload.hpp>

namespace prc {

enum class MessageType : uint8_t {
    REGISTER = 1,
    LINK = 2,
    HEARTBEAT = 3,
    PUBLISH = 4
};

struct Message {
    MessageType msgType;
    uint32_t id;
    uint64_t timestamp;

    Message() {}

    Message(MessageType type, uint32_t id) {
        this->msgType = type;
        this->id = id;
        this->timestamp = 69;  // some chronos voodoo
    }

    Message(std::vector<uint8_t> bytes) { this->fromBytes(bytes); }

    ~Message() {}

    std::vector<uint8_t> toBytes() {
        std::vector<uint8_t> bytes;
        _varToBytes(msgType, bytes);
        _varToBytes(id, bytes);
        _varToBytes(timestamp, bytes);
        _toBytes(bytes);  // Subclass deconstruct
        return bytes;
    }

    bool fromBytes(std::vector<uint8_t> bytes) {
        int offset = 0;
        _varFromBytes(msgType, bytes, offset);
        _varFromBytes(id, bytes, offset);
        _varFromBytes(timestamp, bytes, offset);
        bool success = _fromBytes(bytes, offset);  // Subclass reconstruct
        return success;
    }

   protected:
    virtual void _toBytes(std::vector<uint8_t> &bytes) {}
    virtual bool _fromBytes(std::vector<uint8_t> &bytes, int &offset) {
        return true;
    }

    template <typename T>
    void _varToBytes(const T &var, std::vector<uint8_t> &vect) {
        uint8_t *ptr = (uint8_t *)&var;
        for (int i = 0; i < sizeof(var); i++) {
            vect.push_back(ptr[i]);
        }
    }

    template <typename T>
    void _varFromBytes(T &var, const std::vector<uint8_t> &vect, int &offset) {
        uint8_t *ptr = (uint8_t *)&var;
        for (int i = 0; i < sizeof(var); i++) {
            ptr[i] = vect[i + offset];
        }
        offset += sizeof(var);
    }

    void _stringToBytes(const std::string &str, std::vector<uint8_t> &bytes) {
        for (uint8_t character : str) {
            bytes.push_back(character);
        }
        bytes.push_back('\0');
    }

    std::string _stringFromBytes(const std::vector<uint8_t> &bytes,
                                 int &offset) {
        std::string str = "";
        int i = 0;
        while ((i + offset) < bytes.size()) {
            uint8_t character = bytes[i + offset];
            i++;
            if (character != '\0') {
                str += character;
            } else {
                break;
            }
        }
        offset += i;
        return str;
    }
};

struct RegisterMessage : public Message {
    NodeType nodeType;
    std::string ip;
    uint32_t port;
    std::vector<std::string> topics;

    RegisterMessage() : Message() {}

    RegisterMessage(std::vector<uint8_t> bytes) : Message() {
        this->fromBytes(bytes);
    }

    RegisterMessage(uint32_t id, NodeType nodeType, std::string ip,
                    uint32_t port)
        : Message(MessageType::REGISTER, id) {
        this->nodeType = nodeType;
        this->ip = ip;
        this->port = port;
    }

    ~RegisterMessage() {}

   protected:
    void _toBytes(std::vector<uint8_t> &bytes) {
        _varToBytes(nodeType, bytes);
        _stringToBytes(ip, bytes);
        _varToBytes(port, bytes);
        for (std::string topic : topics) {
            _stringToBytes(topic, bytes);
        }
        bytes.push_back(topics.size());
    }

    bool _fromBytes(std::vector<uint8_t> &bytes, int &offset) {
        _varFromBytes(nodeType, bytes, offset);
        ip = _stringFromBytes(bytes, offset);
        _varFromBytes(port, bytes, offset);

        uint8_t numTopics = bytes.back();
        bytes.pop_back();
        for (int i = 0; i < numTopics; i++) {
            topics.push_back(_stringFromBytes(bytes, offset));
        }
        return true;
    }
};

struct LinkMessage : public Message {
    std::string ip;
    uint32_t port;
    std::string topic;

    LinkMessage() {}
    LinkMessage(uint32_t id, std::string ip, uint32_t port, std::string topic)
        : Message(MessageType::LINK, id) {
        this->ip = ip;
        this->port = port;
        this->topic = topic;
    }
    LinkMessage(std::vector<uint8_t> bytes) { this->fromBytes(bytes); }
    ~LinkMessage() {}

   protected:
    void _toBytes(std::vector<uint8_t> &bytes) {
        _stringToBytes(ip, bytes);
        _varToBytes(port, bytes);
        _stringToBytes(topic, bytes);
    }

    bool _fromBytes(std::vector<uint8_t> &bytes, int &offset) {
        ip = _stringFromBytes(bytes, offset);
        _varFromBytes(port, bytes, offset);
        topic = _stringFromBytes(bytes, offset);
        return true;
    }
};

struct PublishMessage : public Message {
    std::string topic;

    PublishMessage() {}
    PublishMessage(uint8_t id, std::string topic, Payload &pld)
        : Message(MessageType::PUBLISH, id) {
        this->topic = topic;
        pld.toBytes(payload);
    }
    PublishMessage(std::vector<uint8_t> bytes) { this->fromBytes(bytes); }
    ~PublishMessage() {}

    void attachPayload(Payload &pld) {
        payload.clear();
        pld.toBytes(payload);
    }
    void detachPayload(Payload &pld) { pld.fromBytes(payload); }

   protected:
    void _toBytes(std::vector<uint8_t> &bytes) {
        _stringToBytes(topic, bytes);
        for (int i = 0; i < payload.size(); i++) {
            bytes.push_back(payload[i]);
        }
    }
    bool _fromBytes(std::vector<uint8_t> &bytes, int &offset) {
        topic = _stringFromBytes(bytes, offset);
        for (int i = offset; i < bytes.size(); i++) {
            payload.push_back(bytes[i]);
        }
        return true;
    }

   private:
    std::vector<uint8_t> payload;
};

struct HeartbeatMessage : public Message {
    HeartbeatMessage() {}
    HeartbeatMessage(uint8_t id) : Message(MessageType::HEARTBEAT, id) {}
    HeartbeatMessage(std::vector<uint8_t> bytes) { this->fromBytes(bytes); }
    ~HeartbeatMessage() {}
};

}  // namespace prc

#endif