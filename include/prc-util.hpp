#ifndef PROCCOM_UTIL_HPP
#define PROCCOM_UTIL_HPP

#include <prc-common.hpp>

namespace prc {
    int64_t timestamp();
    void stringToBytes(const std::string& str, std::vector<uint8_t>& bytes);
    void stringFromBytes(std::string& var, const std::vector<uint8_t>& bytes, int& offset);

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

    /*------------------------------------------------------------------------------
    Event flag
    ------------------------------------------------------------------------------*/
    class Event {
    private:
        bool flag;
        std::mutex mutex;
        std::condition_variable cv;

    public:
        Event();
        ~Event();

        void set();
        void clear();
        void wait();

    };

}

#endif