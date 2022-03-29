#include "utils/escapedstr.h"
#include <iostream>
#include <fstream>

void
EscapedResolver::parseEscaped(std::vector<std::string> &vec, const std::string &rawStr, const size_t &vecSize,
                              const int *insPos) {
    int curPos = 0;
    size_t size = rawStr.size();
    size_t strInitSize = rawStr.size() / (vecSize - 1);
    for (size_t i = 1; i < size; i++) {
        if (rawStr[i] == '\\') {
            vec[insPos[curPos]] += rawStr[++i];
        } else if (rawStr[i] == '"') {
            curPos++;
            if (curPos < vecSize) vec[insPos[curPos]].reserve(strInitSize);
            i += 2;
        } else vec[insPos[curPos]] += rawStr[i];
    }
}

void EscapedResolver::quotedToStream(std::string &str, std::ofstream &stream) {
    stream << '\'';
    size_t size = str.size();
    for (size_t i = 0; i < size; i++) {
        if (str[i] == '\'') stream << "\\'";
        else stream << str[i];
    }
    stream << '\'';
}

