#ifndef PROCCOM_COMMON_HPP
#define PROCCOM_COMMON_HPP

#include <cstdint>
#include <vector>
#include <map>
#include <deque>
#include <thread>
#include <mutex>
#include <chrono>
#include <functional>



namespace prc {



    /*------------------------------------------------------------------------------
    Node Type
    ------------------------------------------------------------------------------*/
    enum class NodeType : uint8_t { BROKER = 1, PUBLISHER = 2, SUBSCRIBER = 3 };

    class Event {
    private:
        bool flag;
        std::mutex mutex;
        std::condition_variable cv;

    public:
        Event() { flag = false; }
        ~Event() {}

        void set() {
            std::lock_guard g(mutex);
            flag = true;
            cv.notify_all();
        }

        void clear() {
            std::lock_guard g(mutex);
            flag = false;
        }

        void wait() {
            std::unique_lock lock(mutex);
            cv.wait(lock, [this]() { return flag; });
        }

    };

    int64_t timestamp() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); } // chronos voodoo

}  // namespace prc

#endif