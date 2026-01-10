#ifndef ELIZAOS_UUID_GENERATOR_HPP
#define ELIZAOS_UUID_GENERATOR_HPP

#include <string>
#include <random>
#include <sstream>
#include <iomanip>

namespace elizaos {

inline std::string generateUUID() {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis;
    
    uint64_t random1 = dis(gen);
    uint64_t random2 = dis(gen);
    
    random1 = (random1 & 0xFFFFFFFFFFFF0FFFULL) | 0x0000000000004000ULL;
    random2 = (random2 & 0x3FFFFFFFFFFFFFFFULL) | 0x8000000000000000ULL;
    
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    oss << std::setw(8) << ((random1 >> 32) & 0xFFFFFFFF) << '-';
    oss << std::setw(4) << ((random1 >> 16) & 0xFFFF) << '-';
    oss << std::setw(4) << (random1 & 0xFFFF) << '-';
    oss << std::setw(4) << ((random2 >> 48) & 0xFFFF) << '-';
    oss << std::setw(12) << (random2 & 0xFFFFFFFFFFFFULL);
    
    return oss.str();
}

inline std::string generateSimpleUUID() {
    return generateUUID();
}

} // namespace elizaos

#endif
