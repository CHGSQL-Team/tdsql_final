#include "utils/escapedstr.h"

void EscapedResolver::parseEscaped(std::vector<std::string> &vec, std::string &rawStr) {
    int curPos = 0;
    size_t size = rawStr.size();
    for (size_t i = 1; i < size; i++) {
        if (rawStr[i] == '\\') {
            vec[curPos] += rawStr[++i];
        } else if (rawStr[i] == '"') {
            curPos++;
            i += 2;
        } else vec[curPos] += rawStr[i];
    }
}
