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

    std::vector<std::string> sources;
    std::string db_name, table_name;
    TableDescriptor *table;

    WorkDescriptor(std::vector<std::string> _sources, std::string _db_name, std::string _table_name);

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
    std::string name;
    std::vector<Column *> cols;
    std::vector<Column *> primary_keys;
    std::string insert_header;

    ~TableDescriptor();

    template<typename Archive>
    void serialize(Archive &ar, unsigned int version);
};

class Column {
public:
    // Column with **NOT NULL and UNIQUE** will be considered is_primary! (that is the MySQL's idiom)
    // Column with UNIQUE but without NOT NULL will be only considered is_unique
    std::string name;
    bool is_unique;
    bool is_not_null;
    bool is_primary;
    std::string data_type;

    template<typename Archive>
    void serialize(Archive &ar, unsigned int version);
};