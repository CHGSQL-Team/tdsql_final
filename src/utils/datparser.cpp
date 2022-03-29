#include "utils/datparser.h"
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
    boost::escaped_list_separator<char> separator('\\', ',', '\"');
    std::ifstream stream(datPath.c_str());
    std::cout << "datPath: " << datPath.c_str() << std::endl;

    std::mutex vecProt;
    int *insPos = nullptr;
    table->getPhyPosArray(insPos);
    semaphore sem(4);
    auto parseLineToVec = [&](std::vector<std::string> *lines) {
        for (const auto &line: *lines) {
            std::vector<std::string> lineRes;
            lineRes.resize(table->colPhy);
            int pos = 0;
            try {
                Tokenizer tok(line, separator);
                auto tokenEnd = tok.end();
                for (Tokenizer::iterator it = tok.begin(); it != tokenEnd; it++) {
                    lineRes[insPos[pos++]] = *it;
                }
            } catch (boost::wrapexcept<boost::escaped_list_error> &err) {
                std::cout << "Escaped list error! " << line << std::endl;
            }
            Row *row = new Row(std::move(lineRes), source, -1);
            std::lock_guard guard(vecProt);
            row->stamp = stamp++;
            ret.push_back(row);
        }
        sem.release();
    };
    auto *lines = new std::vector<std::string>;
    std::string line;
    boost::asio::thread_pool parserPool(4);
    while (std::getline(stream, line)) {
        lines->reserve(5000);
        lines->push_back(line);
        if (lines->size() >= 5000) {
            sem.acquire();
            boost::asio::post(parserPool, [=] {
                parseLineToVec(lines);
                delete lines;
            });
            lines = new std::vector<std::string>;
        }
    }
    if (!lines->empty()) {
        boost::asio::post(parserPool, [=] {
            parseLineToVec(lines);
            delete lines;
        });
    }
    parserPool.join();

    return ret;
}
