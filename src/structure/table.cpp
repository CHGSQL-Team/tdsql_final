#include "structure/table.h"
#include <iostream>

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
    int printed = 0;
    int print_seq[colDes.size()];
    for (size_t i = 0; i < colDes.size(); i++) {
        print_seq[i] = colDes[i]->mapping;
        std::cout << colDes[i]->name << "\t";
    }
    std::cout << std::endl;
    for (const auto &row: rows) {
        if (!row) continue;
        if (printed >= trunc) break;
//        for (size_t i = 0; i < colDes.size(); i++) {
//            if (row[])
//        }
    }
}
