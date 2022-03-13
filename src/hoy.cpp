#include <prc-publisher.hpp>
#include <iostream>



int main(int argc, char* argv[]) {
    int port;
    std::string id;
    if (argc != 3) {
        std::cout << "usage: " << argv[0] << "<id> <port>\n";
        return 2;
    } else {
        id = argv[1];
        std::string sPort(argv[2]);
        port = std::stoi(sPort);
    }
    prc::Publisher publisher(id, "192.168.1.153", port);
    publisher.addTopic("testTopic");
    publisher.start("192.168.1.153", 6969);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(10s);

    publisher.stop();
    return 0;
}