#include <prc-message.hpp>



prc::Message::Message() {}
prc::Message::Message(MessageType type) { this->msgType = type; }
prc::Message::Message(std::vector<uint8_t>& bytes) { this->fromBytes(bytes); }
prc::Message::~Message() {}

std::vector<uint8_t> prc::Message::toBytes() {
    std::vector<uint8_t> bytes;
    prc::varToBytes(msgType, bytes);
    prc::stringToBytes(id, bytes);
    prc::varToBytes(timestamp, bytes);
    _toBytes(bytes);  // Subclass deconstruct
    return bytes;
}

bool prc::Message::fromBytes(std::vector<uint8_t>& bytes) {
    int offset = 0;
    prc::varFromBytes(msgType, bytes, offset);
    prc::stringFromBytes(id, bytes, offset);
    prc::varFromBytes(timestamp, bytes, offset);
    bool success = _fromBytes(bytes, offset);  // Subclass reconstruct
    return success;
}

void prc::Message::_toBytes(std::vector<uint8_t>& bytes) {}
bool prc::Message::_fromBytes(std::vector<uint8_t>& bytes, int& offset) {
    return true;
}


prc::RegisterMessage::RegisterMessage() : Message(MessageType::REGISTER) {}

prc::RegisterMessage::RegisterMessage(std::vector<uint8_t>& bytes) : Message() { this->fromBytes(bytes); }

prc::RegisterMessage::~RegisterMessage() {}

void prc::RegisterMessage::_toBytes(std::vector<uint8_t>& bytes) {
    prc::varToBytes(nodeType, bytes);
    prc::stringToBytes(ip, bytes);
    prc::varToBytes(port, bytes);
    for (std::string topic : topics) {
        prc::stringToBytes(topic, bytes);
    }
    bytes.push_back(topics.size());
}

bool prc::RegisterMessage::_fromBytes(std::vector<uint8_t>& bytes, int& offset) {
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


prc::PublishMessage::PublishMessage() : Message(MessageType::PUBLISH) {}

prc::PublishMessage::PublishMessage(std::vector<uint8_t>& bytes) : Message() { this->fromBytes(bytes); }

prc::PublishMessage::~PublishMessage() {}

void prc::PublishMessage::attachPayload(Payload& pld) {
    payload.clear();
    pld.toBytes(payload);
}

void prc::PublishMessage::detachPayload(Payload& pld) { pld.fromBytes(payload); }


void prc::PublishMessage::_toBytes(std::vector<uint8_t>& bytes) {
    prc::stringToBytes(topic, bytes);
    for (int i = 0; i < payload.size(); i++) {
        bytes.push_back(payload[i]);
    }
}

bool prc::PublishMessage::_fromBytes(std::vector<uint8_t>& bytes, int& offset) {
    prc::stringFromBytes(topic, bytes, offset);
    for (int i = offset; i < bytes.size(); i++) {
        payload.push_back(bytes[i]);
    }
    return true;
}


prc::HeartbeatMessage::HeartbeatMessage() : Message(MessageType::HEARTBEAT) {}
prc::HeartbeatMessage::HeartbeatMessage(std::vector<uint8_t>& bytes) : Message() { this->fromBytes(bytes); }
prc::HeartbeatMessage::~HeartbeatMessage() {}


prc::UnregisterMessage::UnregisterMessage() : Message(MessageType::HEARTBEAT) {}
prc::UnregisterMessage::UnregisterMessage(std::vector<uint8_t>& bytes) : Message() { this->fromBytes(bytes); }
prc::UnregisterMessage::~UnregisterMessage() {}

