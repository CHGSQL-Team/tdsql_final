#include "utils/datparser.h"
#include "boost/tokenizer.hpp"

#include <iostream>
#include <utility>

typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

DATParser::DATParser(boost::filesystem::path datPath, int source, int &startStamp, Table *table)
        : datPath(std::move(datPath)),
          source(source), stamp(startStamp), table(table) {
}

std::vector<Row *> DATParser::parseData() {
    std::vector<Row *> ret;
    boost::escaped_list_separator<char> separator('\\', ',', '\"');
    std::ifstream stream(datPath.c_str());
    std::cout << "datPath: " << datPath.c_str() << std::endl;
    std::string line;
    Tokenizer tok(line, separator);
    int *insPos = nullptr;
    table->getPhyPosArray(insPos);
    while (std::getline(stream, line)) {
        std::vector<std::string> lineRes;
        lineRes.resize(table->colPhy);
        tok.assign(line);
        int pos = 0;
        for (Tokenizer::iterator it = tok.begin(); it != tok.end(); it++) {
            lineRes[insPos[pos++]] = *it;
        }
        Row *row = new Row(std::move(lineRes), source, stamp++);
        ret.push_back(row);
    }
    return ret;
}
