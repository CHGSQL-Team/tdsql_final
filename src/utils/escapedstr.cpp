#include "utils/escapedstr.h"

void EscapedResolver::parseEscaped(std::vector<std::string> &vec, std::string &rawStr, size_t vecSize) {
    int curPos = 0;
    size_t size = rawStr.size();
    size_t strInitSize = rawStr.size() / (vecSize - 1);
    for (size_t i = 1; i < size; i++) {
        if (rawStr[i] == '\\') {
            vec[curPos] += rawStr[++i];
        } else if (rawStr[i] == '"') {
            curPos++;
            if (curPos < vecSize) vec[curPos].reserve(strInitSize);
            i += 2;
        } else vec[curPos] += rawStr[i];
    }
}

void EscapedResolver::quotedToStream(std::string &str, std::ofstream &stream) {

}

