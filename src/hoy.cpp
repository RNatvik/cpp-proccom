#include <prc-common.hpp>
#include <thread>
#include <iostream>

prc::Event event;

void foo() {
    std::cout << std::this_thread::get_id() << " Waiting for event in foo()" << std::endl;
    event.wait();
    event.clear();
    std::cout << std::this_thread::get_id() << " Doing work in foo()" << std::endl;
    
    using namespace std::chrono_literals;
    std::this_thread::sleep_for(3000ms);
    std::cout << std::this_thread::get_id() << " Done" << std::endl;
    event.set();
}


int main() {
    event.clear();

    std::thread thr1 = std::thread(foo);
    std::thread thr2 = std::thread(foo);
    std::thread thr3 = std::thread(foo);

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1000ms);
    std::cout << "Main thread setting event" << std::endl;
    event.set();

    if (thr1.joinable()) thr1.join();
    if (thr2.joinable()) thr2.join();
    if (thr3.joinable()) thr3.join();
}