#include <cstddef>
#include <cstdint>
#include "underlying/event_internal.h"
#include "binbuffer.h"
#include <string>
#include <vector>

class EventHeader;

class Event {
public:
    unsigned long data_size{};

    EventHeader *header;

    explicit Event(EventHeader *header);

    virtual void _print() = 0;

    virtual void _print_header();

    virtual void decode(BinBuffer *buffer) = 0;
};

class UnknownEvent : public Event {
public:
    using Event::Event;

    void decode(BinBuffer *buffer);

    void _print() override;
};

class TableMapEvent : public Event {
public:
    uint64_t tableId;
    std::string database;
    std::string table;

    uint64_t numberOfColumns;
    std::vector<uint8_t> columnTypes;
    uint64_t metadataLength;
    std::vector<uint32_t> columnMetadata;
    std::vector<bool> ColumnNullability;

    using Event::Event;

    void decode(BinBuffer *buffer);

    void _print() override;

    void _readMetadata(BinBuffer *buffer);
};

class GTIDEvent : public Event {
public:
    uint8_t flags = 0;
    unsigned char sid[17];
    uint64_t gno = 0;

    using Event::Event;

    void decode(BinBuffer *buffer);

    void _print() override;
};

class EventReader {
private:
    unsigned long size;
    BinBuffer *buffer;

public:
    EventReader();

    EventReader(unsigned long size, const unsigned char *buffer);

    void set(unsigned long _size, const unsigned char *_buffer);

    Event *process();
};

class EventHeader {
public:
    unsigned long size;
    Log_event_type type;
    uint32_t server_id;
    uint32_t event_len;
    uint32_t next_position;
    uint16_t flags;

    void _print() const;
};