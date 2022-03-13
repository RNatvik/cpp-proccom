#include <prc-broker.hpp>


int main() {
    prc::Broker broker("MainBroker", "192.168.1.153", 6969, 15000);
    broker.start();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(240s);

    return 0;
}