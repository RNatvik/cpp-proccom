#include <prc-util.hpp>

int64_t prc::timestamp() {
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void prc::stringToBytes(const std::string& str, std::vector<uint8_t>& bytes) {
    for (uint8_t character : str) {
        bytes.push_back(character);
    }
    bytes.push_back('\0');
}

void prc::stringFromBytes(std::string& var, const std::vector<uint8_t>& bytes, int& offset) {
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

prc::Event::Event() { flag = false; }
prc::Event::~Event() {}

void prc::Event::set() {
    std::lock_guard g(mutex);
    flag = true;
    cv.notify_all();
}

void prc::Event::clear() {
    std::lock_guard g(mutex);
    flag = false;
}

void prc::Event::wait() {
    std::unique_lock lock(mutex);
    cv.wait(lock, [this]() { return flag; });
}