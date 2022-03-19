#pragma once

#include <string>
#include <vector>
#include <map>
#include <boost/serialization/split_member.hpp>


class Column;

class TableDescriptor;

class WorkDescriptor {
public:
    friend class boost::serialization::access;

    std::string db_name, table_name;
    TableDescriptor *table;

    WorkDescriptor(std::string _db_name, std::string _table_name);

    WorkDescriptor();

    ~WorkDescriptor();

    template<typename Archive>
    void serialize(Archive &ar, unsigned int version);

};

class Works {
public:

    friend class boost::serialization::access;

    std::map<std::pair<std::string, std::string>, WorkDescriptor *> data;
    typedef std::map<std::pair<std::string, std::string>, WorkDescriptor *>::const_iterator const_iterator;

    const_iterator begin() const { return data.begin(); }

    const_iterator end() const { return data.end(); }

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

class TableDescriptor {
public:

};

class Column {
public:

};