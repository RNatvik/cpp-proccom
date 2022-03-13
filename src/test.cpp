#include <prc-subscriber.hpp>

#define CUSTOM_TOPIC "testTopic"

struct CustomPayload : public prc::Payload {
    std::string message;

    virtual void toBytes(std::vector<uint8_t>& bytes) {
        _stringToBytes(this->message, bytes);
    }
    virtual bool fromBytes(std::vector<uint8_t>& bytes) {
        int offset = 0;
        this->message = _stringFromBytes(bytes, offset);
        return true;
    }
};


void publishHandler(prc::PublishMessage& msg) {
    if (msg.topic == CUSTOM_TOPIC) {
        CustomPayload pld;
        msg.detachPayload(pld);
        std::cout << pld.message << std::endl << std::endl;
    }
    else {
        std::cout << "wrong topic" << std::endl;
    }
}

int main() {
    prc::Subscriber subscriber("testSub", "192.168.1.153", 6971);
    subscriber.attachPublishHandler(publishHandler);
    subscriber.addTopic(CUSTOM_TOPIC);
    subscriber.start("192.168.1.153", 6969);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(60s);

    // prc::NodeLookup nodes;
    // prc::NodeInfo info;
    // std::vector<std::string> topics;
    // topics.push_back("topic1");
    // topics.push_back("topic2");

    // info.id = "test";
    // info.ip = "127.0.0.1";
    // info.port = 10;
    // info.type = prc::NodeType::PUBLISHER;
    // info.heartbeat = prc::timestamp();
    // info.topics = topics;   

    // nodes.addNode(info);

    // prc::NodeInfo* ptr;
    // nodes.getNodeByID("test", ptr);

    // auto vect = nodes.getNodes();
    // auto vect2 = nodes.getPublishers();
    // auto vect3 = nodes.getSubscribers();
    // auto vect5 = nodes.getPublishersByTopic("topic1");
    // auto vect6 = nodes.getSubscribersByTopic("topic1");
    

}