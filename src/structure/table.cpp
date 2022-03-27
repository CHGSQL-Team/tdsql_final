#include "structure/table.h"
#include "boost/container_hash/hash.hpp"
#include <iostream>
#include "utils/uniformlog.h"
#include <utility>
#include <iomanip>

inline bool compRows(Row *x, Row *y) {
    if (!x && y) return true;
    if (x && !y) return false;
    if (!x) return false;
    return x->data[0].size() < y->data[0].size() || (x->data[0].size() == y->data[0].size() && x->data[0] < y->data[0]);
}

Table::Table(WorkDescriptor *workDes) : workDes(workDes) {

}

void Table::addColumn(ColumnDescriptor *newCol, std::string *after) {
    if (after) {
        auto baseCol = std::find_if(colDes.begin(), colDes.end(),
                                    [=](const ColumnDescriptor *x) { return x->name == *after; });
        if (baseCol == colDes.end()) throw std::runtime_error("Can not find col to insert!");
        colDes.insert(++baseCol, newCol);
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
    if (!indexs.empty() && indexs[0]->isTemp)
        dropUniqueIndex(false, true, ""); // Don't worry, new temp index will be created!
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
    if (indexs.size() != 1) throw std::runtime_error("Index size should be 1!");
    auto index = *(indexs.begin());
    for (const auto &row: newRows) {
        insertRow(row, index);
    }
}

inline void Table::insertRow(Row *newRow, UniqueIndex *index) {
    if (index->checkRow(newRow) == 0) {
        index->updateRow(newRow);
        newRow->idxInsideTable = rows.size();
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

void Table::dumpToFile(const boost::filesystem::path &path) {
    std::ofstream stream(path.c_str());
    int *phyPos = nullptr;
    size_t size = getPhyPosArray(phyPos);
    for (auto row: rows) {
        if (!row) continue;
        for (int i = 0; i < size; i++) {
            stream << std::quoted(row->data[phyPos[i]], '\'', '\\');
            if (i != size - 1) stream << ",";
        }
        stream << std::endl;
    }
    stream.close();
}

void Table::doRowReplace(Row *oldRow, Row *newRow) {
    rows[oldRow->idxInsideTable] = newRow;
    newRow->idxInsideTable = oldRow->idxInsideTable;
    delete oldRow;
}

void Table::dropColumn(const std::string &colName) {
    auto col = std::find_if(colDes.begin(), colDes.end(), [&](const ColumnDescriptor *x) {
        return x->name == colName;
    });
    if (col == colDes.end()) throw std::runtime_error("Can not find column to drop!");
    for (auto index = indexs.begin(); index != indexs.end();) {
        (*index)->deleteCol(*col);
        if ((*index)->cols.empty()) {
            delete (*index);
            index = indexs.erase(index);
        } else ++index;
    }
    colDes.erase(col);
    if (indexs.empty())
        indexs.push_back(new UniqueIndex(this));

    nameToColDes.erase(colName);

}

void Table::dropUniqueIndex(bool isPrimary, bool isTemp, const std::string &indexName) {
    auto toDrop = std::find_if(indexs.begin(), indexs.end(), [&](UniqueIndex *index) {
        if (isPrimary) return index->isPrimary;
        else if (isTemp)
            return index->isTemp;
        else
            return index->name == indexName;
    });
    if (toDrop == indexs.end()) {
        std::cout << "[Table] Can not find index to drop. May be the index is ignored at the very first. It is okay!"
                  << std::endl;
        return;
    }
    indexs.erase(toDrop);
    if (indexs.empty()) indexs.push_back(new UniqueIndex(this));
}

Table::~Table() {
    for (const auto &row: rows) {
        delete row;
    }
    for (const auto &index: indexs) {
        delete index;
    }
}

void Table::optimizeTableForDump() {
    if (colDes.empty()) return;
    std::sort(rows.begin(), rows.end(), compRows);
}

Row::Row(std::vector<std::string> &&data, int source, int stamp) : data(std::move(data)), source(source), stamp(stamp) {
}

int UniqueIndex::checkRow(Row *row) {
    auto it = hash.find(getHashOfRow(row));
    if (it != hash.end()) {
        auto oldRow = it->second;

        if (oldRow->data[table->updCol->mapping] == row->data[table->updCol->mapping]) {
            if (row->source < oldRow->source) {
                it->second = row;
                table->doRowReplace(oldRow, row);
                return 1;
            } else if (row->source > oldRow->source) {
                return -1;
            } else {
                if (row->stamp > oldRow->stamp) {
                    it->second = row;
                    table->doRowReplace(oldRow, row);
                    return 1;
                } else return -1;
            }
        } else if (oldRow->data[table->updCol->mapping] < row->data[table->updCol->mapping]) {
            it->second = row;
            table->doRowReplace(oldRow, row);
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
    setHashPhy();
    reCompute();
}

UniqueIndex::UniqueIndex(Table *table) : table(table), name("___TEMP_PRIMARY"), isPrimary(false), isTemp(true) {
    std::copy(table->colDes.begin(), table->colDes.end(), std::inserter(cols, cols.end()));
    cols.erase(table->updCol);
    setHashPhy();
    reCompute();
}

void UniqueIndex::reCompute() {
    hash.clear();
    size_t rowCount = table->rows.size();
    for (size_t i = 0; i < rowCount; i++) {
        int checkRes = checkRow(table->rows[i]);
        if (checkRes == 0) {
            updateRow(table->rows[i]);
        } else table->rows[i] = nullptr;
    }
}

inline void UniqueIndex::updateRow(Row *row) {
    hash[getHashOfRow(row)] = row;
}

size_t UniqueIndex::getHashOfRow(Row *row) const {
    size_t size = cols.size();
    size_t hashValue = 2022;
    for (size_t i = 0; i < size; i++)
        boost::hash_combine(hashValue, row->data[hashPhy[i]]);
    return hashValue;
}

void UniqueIndex::setHashPhy() {
    delete[] hashPhy;
    hashPhy = new int[cols.size()];
    int pos = 0;
    for (auto col: cols)
        hashPhy[pos++] = col->mapping;
}

void UniqueIndex::deleteCol(ColumnDescriptor *delCol) {
    auto indexTargetCol = cols.find(delCol);
    if (indexTargetCol != cols.end()) {
        cols.erase(indexTargetCol);
        setHashPhy();
        reCompute();
    }
}
