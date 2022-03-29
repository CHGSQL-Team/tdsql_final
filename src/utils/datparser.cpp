#include "utils/datparser.h"
#include "utils/escapedstr.h"
#include "utils/semaphore.h"
#include "boost/tokenizer.hpp"
#include "boost/asio/thread_pool.hpp"
#include "boost/asio/post.hpp"

#include <iostream>
#include <utility>
#include <mutex>

typedef boost::tokenizer<boost::escaped_list_separator<char> > Tokenizer;

DATParser::DATParser(boost::filesystem::path datPath, int source, int &startStamp, Table *table)
        : datPath(std::move(datPath)),
          source(source), stamp(startStamp), table(table) {
}

std::vector<Row *> DATParser::parseData() {
    std::vector<Row *> ret;
    std::ifstream stream(datPath.c_str());
    std::string line;
    int *insPos = nullptr;
    size_t vecSize = table->getPhyPosArray(insPos);
    while (std::getline(stream, line)) {
        std::vector<std::string> lineRes;
        lineRes.resize(table->colPhy);
        EscapedResolver::parseEscaped(lineRes, line, vecSize, insPos);
        Row *row = new Row(std::move(lineRes), source, stamp++);
        ret.push_back(row);
    }
    delete insPos;
    return ret;
}
