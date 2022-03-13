#include <prc-publisher.hpp>
#include <prc-util.hpp>
#include <iostream>

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

int main(int argc, char* argv[]) {
    std::vector<uint8_t> bytes;
    bool test = true;

    prc::varToBytes(test, bytes);

    int port;
    std::string id;
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << "<id> <port>\n";
        return 2;
    }
    else {
        id = argv[1];
        std::string sPort(argv[2]);
        port = std::stoi(sPort);
    }
    prc::Publisher publisher(id, "192.168.1.153", port);
    publisher.addTopic(CUSTOM_TOPIC);
    publisher.addTopic("testTopic69");
    publisher.start("192.168.1.153", 6969);


    using namespace std::chrono_literals;
    std::this_thread::sleep_for(5s);

    CustomPayload pld;
    pld.message = "Hello";
    for (int i = 0; i < 50; i++) {
        publisher.publish(CUSTOM_TOPIC, pld);
        std::this_thread::sleep_for(1s);
    }
    std::this_thread::sleep_for(10s);

    publisher.stop();
    return 0;
}