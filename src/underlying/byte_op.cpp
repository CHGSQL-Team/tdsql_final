#include "underlying/byte_op.h"
#include "binbuffer.h"
#include <cstddef>
#include <cstdint>
#include <algorithm>

std::vector<bool> readBitSet(BinBuffer *buffer, uint64_t length, bool bigEndian) {
    std::vector<uint8_t> bytes = std::move(buffer->readToVector((length + 7) >> 3));
    if (!bigEndian) std::reverse(bytes.begin(),bytes.end());
    std::vector<bool> result;
    for (int i = 0; i < length; i++) {
        if ((bytes[i >> 3] & (1 << (i % 8))) != 0) {
            result.push_back(true);
        } else result.push_back(false);
    }
    return result;
}