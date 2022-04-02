#pragma once

#include <string>
#include <vector>
#include <map>
#include "boost/serialization/split_member.hpp"
#include "boost/filesystem.hpp"
#include "table.h"


class ColumnDescriptor;

class Table;


class WorkDescriptor {
public:
    friend class boost::serialization::access;

    std::string db_name, table_name;

    boost::filesystem::path binlogPath;

    Table *table;

    int stateCount;

    WorkDescriptor(std::string _db_name, std::string _table_name, boost::filesystem::path binlogPath, int stateCount);

    ~WorkDescriptor();

    template<typename Archive>
    void serialize(Archive &ar, unsigned int version);

};

class Works {
public:

    friend class boost::serialization::access;

    std::map<std::pair<std::string, std::string>, WorkDescriptor *> data;
    typedef std::map<std::pair<std::string, std::string>, WorkDescriptor *>::const_iterator const_iterator;

    typedef std::map<std::pair<std::string, std::string>, WorkDescriptor *>::const_reverse_iterator const_reverse_iterator;

    [[nodiscard]] const_iterator begin() const { return data.begin(); }

    [[nodiscard]] const_iterator end() const { return data.end(); }

    [[nodiscard]] const_reverse_iterator rbegin() const { return data.rbegin(); }

    [[nodiscard]] const_reverse_iterator rend() const { return data.rend(); }

    WorkDescriptor *&operator[](std::pair<std::string &, std::string &> &in) { return data[in]; }

    WorkDescriptor *&operator[](const std::pair<std::string, std::string> &in) { return data[in]; }

    const_iterator find(std::pair<std::string &, std::string &> &in) { return data.find(in); }

    const_iterator find(const std::pair<std::string, std::string> &in) { return data.find(in); }

    template<typename Archive>
    void save(Archive &ar, unsigned int version) const;

    template<typename Archive>
    void load(Archive &ar, unsigned int version);

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};

class ColumnDescriptor {
public:
    ColumnDescriptor(std::string name, std::string *def);

    std::string name;
    int mapping = -1;
    std::string *def = nullptr;

    bool operator<(const ColumnDescriptor &t) const;
};