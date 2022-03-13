#ifndef PROCCOM_UTIL_HPP
#define PROCCOM_UTIL_HPP

#include <prc-common.hpp>

namespace prc {
    int64_t timestamp() { return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count(); } // chronos voodoo

    template <typename T>
    void varToBytes(const T& var, std::vector<uint8_t>& vect) {
        uint8_t* ptr = (uint8_t*)&var;
        for (int i = 0; i < sizeof(var); i++) {
            vect.push_back(ptr[i]);
        }
    }

    template <typename T>
    void varFromBytes(T& var, const std::vector<uint8_t>& vect, int& offset) {
        uint8_t* ptr = (uint8_t*)&var;
        for (int i = 0; i < sizeof(var); i++) {
            ptr[i] = vect[i + offset];
        }
        offset += sizeof(var);
    }

    void stringToBytes(const std::string& str, std::vector<uint8_t>& bytes) {
        for (uint8_t character : str) {
            bytes.push_back(character);
        }
        bytes.push_back('\0');
    }

    void stringFromBytes(std::string& var, const std::vector<uint8_t>& bytes, int& offset) {
        std::string str = "";
        int i = 0;
        while ((i + offset) < bytes.size()) {
            uint8_t character = bytes[i + offset];
            i++;
            if (character != '\0') {
                str += character;
            }
            else {
                break;
            }
        }
        offset += i;
        var = str;
    }

    /*------------------------------------------------------------------------------
    Event flag
    ------------------------------------------------------------------------------*/
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

}

#endif