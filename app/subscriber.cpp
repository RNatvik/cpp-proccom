#include <prc-subscriber.hpp>

#define CUSTOM_TOPIC "testTopic"

struct CustomPayload : public prc::Payload {
    std::string message;

    virtual void toBytes(std::vector<uint8_t>& bytes) {
        prc::stringToBytes(this->message, bytes);
    }
    virtual bool fromBytes(std::vector<uint8_t>& bytes) {
        int offset = 0;
        prc::stringFromBytes(this->message, bytes, offset);
        return true;
    }
};


void publishHandler(prc::PublishMessage& msg) {
    if (msg.topic == CUSTOM_TOPIC) {
        CustomPayload pld;
        msg.detachPayload(pld);
        auto currentTime = prc::timestamp();
        std::cout << pld.message << " (" << msg.timestamp << ")" << std::endl << std::endl;
        std::cout << "Diff: " << currentTime - msg.timestamp << std::endl;
    }
    else {
        std::cout << "wrong topic" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    int port;
    std::string id;
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << " <id> <port>\n";
        return 2;
    }
    else {
        id = argv[1];
        std::string sPort(argv[2]);
        port = std::stoi(sPort);
    }
    
    prc::Subscriber subscriber(id, "127.0.0.1", port);
    subscriber.attachPublishHandler(publishHandler);
    subscriber.addTopic(CUSTOM_TOPIC);
    subscriber.addTopic("testTopic69");
    subscriber.start("127.0.0.1", 6969);

    std::string str;
    std::cout << "Press ENTER to quit: ";
    while (std::getline(std::cin, str)) {
        if (str.empty()) break;
    }

    subscriber.stop();

}