#include <cstring>
#include "binbuffer.h"

BinBuffer::BinBuffer(const unsigned char *origin, unsigned long size) : origin(origin), size(size), buffer(origin) {
    offset = 0;
}

uint64_t BinBuffer::readPackedInteger() {
    uint64_t value = 0;
    if (buffer[0] < 0xfb) {
        value = skip(1)[0];
    } else {
        skip(1);
        if (buffer[0] == 0xfc)
            readRaw(&value, 2);
        else if (buffer[0] == 0xfd)
            readRaw(&value, 3);
        else if (buffer[0] == 0xfe)
            readRaw(&value, 8);
    }
    return value;
}

inline void BinBuffer::readRaw(void *dst, size_t length) {
    memcpy(dst, skip(length), length);
}

uint8_t BinBuffer::readByte() {
    return skip(1)[0];
}

inline const unsigned char *BinBuffer::skip(size_t length) {
    const unsigned char *ret = buffer;
    offset += length;
    buffer += length;
    return ret;
}

uint16_t BinBuffer::read2Byte() {
    uint16_t value = 0;
    readRaw(&value, 2);
    return value;
}

void BinBuffer::seek(size_t position) {
    offset = position;
    buffer = origin + position;
}

uint32_t BinBuffer::read4Byte() {
    uint32_t value = 0;
    readRaw(&value, 4);
    return value;
}

uint64_t BinBuffer::read6Byte() {
    uint64_t value = 0;
    readRaw(&value, 6);
    return value;
}

uint64_t BinBuffer::read8Byte() {
    uint64_t value = 0;
    readRaw(&value, 8);
    return value;
}
