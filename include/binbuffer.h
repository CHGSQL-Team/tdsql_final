#pragma once

#include <cstdint>
#include <cstddef>
#include <string>
#include <vector>


class BinBuffer {
private:
    const unsigned char *origin;
    const unsigned char *buffer;
    unsigned long size;
    size_t offset;
public:
    BinBuffer(const unsigned char *origin, unsigned long size);

    uint64_t readPackedInteger();

    void readRaw(void *dst, size_t length);

    void seek(size_t position);

    const unsigned char *skip(size_t length); // returns pointer before skipping

    uint8_t readByte();

    uint16_t read2Byte();

    uint16_t read2ByteInBigEndian();

    uint32_t read4Byte();

    uint64_t read6Byte();

    uint64_t read8Byte();

    std::string readZeroTerminatedString();

    std::vector<uint8_t> readToVector(size_t length);

};