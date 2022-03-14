#include <prc-broker.hpp>


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
    prc::Broker broker("Broker", "127.0.0.1", port, 15000);
    broker.start();

    std::string str;
    std::cout << "Press ENTER to quit: ";
    while (std::getline(std::cin, str)) {
        if (str.empty()) break;
    }

    broker.stop();

    return 0;
}