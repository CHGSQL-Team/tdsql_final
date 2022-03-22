#include "structure/table.h"
#include "boost/container_hash/hash.hpp"
#include <iostream>
#include "utils/uniformlog.h"
#include <utility>
#include <iomanip>

Table::Table(WorkDescriptor *workDes) : workDes(workDes) {

}

void Table::addColumn(ColumnDescriptor *newCol, std::string *after) {
    if (after) {
        auto baseCol = std::find_if(colDes.begin(), colDes.end(),
                                    [=](const ColumnDescriptor *x) { return x->name == *after; });
        if (baseCol == colDes.end()) throw std::runtime_error("Can not find col to insert!");
        colDes.insert(baseCol, newCol);
    } else {
        colDes.push_back(newCol);
    }
    nameToColDes[newCol->name] = newCol;
    newCol->mapping = colPhy++;
    if (newCol->name == "updated_at") updCol = newCol;
    for (const auto &row: rows) {
        if (row) {
            if (newCol->def) {
                row->data.push_back(*newCol->def);
            } else {
                row->data.emplace_back("");
            }
        }
    }
}

void Table::print(int trunc) {
    UniformLog log("Print Table", workDes->db_name + "/" + workDes->table_name);
    std::cout << "Showing indexs" << std::endl;
    for (auto index: indexs) {
        std::cout << "Name: " << index->name << ", Cols: ";
        for (auto col: index->cols) {
            std::cout << col->name << ",";
        }
        std::cout << std::endl;
    }
    int printed = 0;
    int print_seq[colDes.size()];
    std::cout << "-------";
    for (size_t i = 0; i < colDes.size(); i++) {
        print_seq[i] = colDes[i]->mapping;
        std::cout << colDes[i]->name << " ";
    }
    std::cout << "-------" << std::endl;
    for (const auto &row: rows) {
        if (!row) continue;
        if (printed >= trunc) break;
        for (auto &col: colDes) {
            std::cout << row->data[col->mapping] << "\t";
        }
        std::cout << "\t | Src=" << row->source << ", Stamp=" << row->stamp;
        printed++;
        std::cout << std::endl;
    }
}

void Table::insertRows(std::vector<Row *> &newRows) {
    for (const auto &row: newRows) {
        insertRow(row);
    }
}

inline void Table::insertRow(Row *newRow) {
    if (indexs.size() != 1) throw std::runtime_error("Index size should be 1!");
    auto index = *(indexs.begin());
    if (index->checkRow(newRow) == 0) {
        index->updateRow(newRow);
        rows.push_back(newRow);
    }
}

size_t Table::getPhyPosArray(int *&array) {
    size_t size = colDes.size();
    array = new int[size];
    for (size_t i = 0; i < colDes.size(); i++) {
        array[i] = colDes[i]->mapping;
    }
    return size;
}

void Table::addUniqueIndex(const std::string &name, const std::set<std::string> &colNames, bool isPrimary) {

}

void Table::dumpToFile(boost::filesystem::path path) {
    std::ofstream stream(path.c_str());
    int *phyPos = nullptr;
    size_t size = getPhyPosArray(phyPos);
    for (auto row: rows) {
        for (int i = 0; i < size; i++) {
            stream << std::quoted(row->data[phyPos[i]]);
            if (i != size - 1) stream << ",";
        }
        stream << std::endl;
    }
}

Row::Row(std::vector<std::string> &&data, int source, int stamp) : data(std::move(data)), source(source), stamp(stamp) {
}

int UniqueIndex::checkRow(Row *row) {
    size_t size = cols.size();
    size_t hashValue = 2022;
    for (size_t i = 0; i < size; i++)
        boost::hash_combine(hashValue, row->data[hashPhy[i]]);
    auto it = hash.find(hashValue);
    if (it != hash.end()) {
        auto oldRow = it->second;
        if (oldRow->data[table->updCol->mapping] == row->data[table->updCol->mapping]) {
            if (row->source < oldRow->source) {
                it->second = row;
                return 1;
            } else if (row->source > oldRow->source) {
                return -1;
            } else {
                if (row->stamp > oldRow->stamp) {
//                    std::cout << "Clearing using STAMP" << row->data[4] << "STAMP IS " << row->stamp << " "
//                              << oldRow->data[4] << "STAMP IS " << oldRow->stamp << std::endl;
                    it->second = row;
//                    std::cout << "NEW ROW IS " << it->second->data[4] << std::endl;
                    return 1;
                } else return -1;
            }
        } else if (oldRow->data[table->updCol->mapping] < row->data[table->updCol->mapping]) {
            it->second = row;
            return 1;
        } else return -1;
    } else return 0;
}

UniqueIndex::UniqueIndex(Table *table, std::string name, const std::set<std::string> &colStrs, bool isPrimary) : name(
        std::move(name)), isPrimary(isPrimary), table(table), isTemp(false) {

    for (const auto &colStr: colStrs) {
        cols.insert(*std::find_if(table->colDes.begin(), table->colDes.end(), [=](const ColumnDescriptor *col) {
            return col->name == colStr;
        }));
    }
    hashPhy = new int[cols.size()];
    int pos = 0;
    for (auto col: cols)
        hashPhy[pos++] = col->mapping;
    reCompute();
}

UniqueIndex::UniqueIndex(Table *table) : table(table), name("___TEMP_PRIMARY"), isPrimary(false), isTemp(true) {
    std::copy(table->colDes.begin(), table->colDes.end(), std::inserter(cols, cols.end()));
    hashPhy = new int[cols.size()];
    int pos = 0;
    for (auto col: cols)
        hashPhy[pos++] = col->mapping;
    reCompute();
}

void UniqueIndex::reCompute() {

}

void UniqueIndex::updateRow(Row *row) {
    size_t size = cols.size();
    size_t hashValue = 2022;
    for (size_t i = 0; i < size; i++)
        boost::hash_combine(hashValue, row->data[hashPhy[i]]);
    hash[hashValue] = row;
}
