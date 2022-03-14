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
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " <port>\n";
        return 2;
    }
    else {
        std::string sPort(argv[1]);
        port = std::stoi(sPort);
    }
    
    prc::Subscriber subscriber("testSub", "192.168.1.153", 6971);
    subscriber.attachPublishHandler(publishHandler);
    subscriber.addTopic(CUSTOM_TOPIC);
    subscriber.addTopic("testTopic69");
    subscriber.start("192.168.1.153", 6969);

    std::string str;
    std::cout << "Press ENTER to quit: ";
    while (std::getline(std::cin, str)) {
        if (str.empty()) break;
    }

    subscriber.stop();

}