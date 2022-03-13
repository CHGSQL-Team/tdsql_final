#include "event.h"
#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include "descriptor.h"
#include "binbuffer.h"

std::string convert_sid(const unsigned char *sid) {
    std::stringstream ss;
    for (int i = 0; i < 16; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << int(sid[i]);
        if (i == 3 || i == 5 || i == 7 || i == 9) ss << "-";
    }
    return ss.str();
}

size_t fill_col_used(std::vector<int> &col_used, uint64_t column_cnt, const unsigned char *buffer) {
    for (int i = 0; i < column_cnt; i++) {
        int outer_idx = i / 8;
        int inner_idx = i % 8;
        if (buffer[outer_idx] & (1u << inner_idx)) {
            col_used.push_back(i);
        }
    }
    return (column_cnt + 7) / 8;
}


void decodeWriteRow(const unsigned char *buffer, uint32_t event_len) {
    /*uint32_t data_len = event_len - 19;
    uint64_t table_id = 0;
    uint64_t column_cnt = 0;
    memcpy(&table_id, buffer, 6);
    size_t buf_offset = 10;
    buf_offset += getPacked(buffer + buf_offset, &column_cnt);
    std::cerr << "table id=" << table_id << ",column cnt=" << column_cnt << std::endl << "col used=";

    std::vector<int> col_used;
    buf_offset += fill_col_used(col_used, column_cnt, buffer + buf_offset);
    for (auto i: col_used) {
        std::cerr << i << ",";
    }
    std::cerr << std::endl;*/

}

void decodeTablemap(const unsigned char *buffer, uint32_t event_len) {
    uint64_t table_id = 0;
}

void GTIDEvent::decode(BinBuffer *buffer) {
    flags = buffer->readByte();
    buffer->readRaw(sid, 16);
    sid[16] = '\0';
    gno = buffer->read8Byte();

}

void GTIDEvent::_print() {
    std::cerr << "GTID: " << convert_sid(sid) << ":" << gno << std::endl;
}


EventReader::EventReader(unsigned long size, const unsigned char *buffer) : EventReader() {
    this->set(size, buffer);
}

Event *EventReader::process() {
    auto *header = new EventHeader;
    Event *event = nullptr;
    buffer->skip(5);
    header->type = (Log_event_type) buffer->readByte();
    header->server_id = buffer->read4Byte();
    header->event_len = buffer->read4Byte();
    header->next_position = buffer->read4Byte();
    header->flags = buffer->read2Byte();
    //if (header->type == Log_event_type::WRITE_ROWS_EVENT)
    if (header->type == Log_event_type::TABLE_MAP_EVENT) event = new TableMapEvent(header);
    if (header->type == Log_event_type::GTID_LOG_EVENT) event = new GTIDEvent(header);
    buffer->seek(20);
    if (event) event->decode(buffer);
    if (!event) event = new UnknownEvent(header);
    return event;
}


EventReader::EventReader() : size(0), buffer(nullptr) {

}

void EventReader::set(unsigned long _size, const unsigned char *_buffer) {
    size = _size;
    buffer = new BinBuffer(_buffer, size);
}

void EventHeader::_print() const {
    std::cerr << "Event type = " << get_type_str(type) << ", "
              << "server id = " << server_id << ", "
              << "event len = " << event_len << ", "
              << "next position = " << next_position << ", "
              << "flags = " << flags
              << std::endl;
}

Event::Event(EventHeader *header) : header(header) {

}

void Event::_print_header() {
    header->_print();
}

void UnknownEvent::decode(BinBuffer *buffer) {}

void UnknownEvent::_print() {
    std::cerr << "No implemented Event" << std::endl;
}

void TableMapEvent::decode(BinBuffer *buffer) {

}

void TableMapEvent::_print() {

}
