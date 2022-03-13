#include <prc-broker.hpp>


int main() {
    prc::Broker broker("MainBroker", "127.0.0.1", 6969, 15000);
    broker.start();

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(240s);

    return 0;
}