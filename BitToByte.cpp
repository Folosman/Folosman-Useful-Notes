#include <iostream>
#include <vector>
#include <cstdint>

std::vector<uint8_t> bitsToBytes(const std::vector<bool>& bits) {
    std::vector<uint8_t> bytes;
    size_t numBytes = (bits.size() + 7) / 8; 

    bytes.reserve(numBytes);

    for (size_t i = 0; i < numBytes; ++i) {
        uint8_t byte = 0;
        for (size_t bit = 0; bit < 8; ++bit) {
            size_t bitIndex = i * 8 + bit;
            if (bitIndex < bits.size() && bits[bitIndex]) {
                byte |= (1 << bit);
            }
        }
        bytes.push_back(byte);
    }

    return bytes;
}