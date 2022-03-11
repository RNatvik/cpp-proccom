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



}  // namespace prc

#endif