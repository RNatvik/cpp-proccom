#ifndef PROCCOM_PAYLOAD_HPP
#define PROCCOM_PAYLOAD_HPP

#include <prc-common.hpp>

namespace prc {
    struct Payload {
    public:
        virtual void toBytes(std::vector<uint8_t>& bytes) = 0;
        virtual bool fromBytes(std::vector<uint8_t>& bytes) = 0;

    protected:
        template <typename T>
        void _toBytes(const T& var, std::vector<uint8_t>& vect) {
            uint8_t* ptr = (uint8_t*)&var;
            for (int i = 0; i < sizeof(var); i++) {
                vect.push_back(ptr[i]);
            }
        }

        template <typename T>
        void _fromBytes(T& var, const std::vector<uint8_t>& vect, int& offset) {
            uint8_t* ptr = (uint8_t*)&var;
            for (int i = 0; i < sizeof(var); i++) {
                ptr[i] = vect[i + offset];
            }
            offset += sizeof(var);
        }

        void _stringToBytes(const std::string& str, std::vector<uint8_t>& bytes) {
            for (uint8_t character : str) {
                bytes.push_back(character);
            }
            bytes.push_back('\0');
        }

        std::string _stringFromBytes(const std::vector<uint8_t>& bytes,
            int& offset) {
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
            return str;
        }
    };

}  // namespace prc

#endif