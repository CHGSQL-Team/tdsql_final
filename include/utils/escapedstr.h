#pragma once

#include <vector>
#include <string>

class EscapedResolver {
public:
    static void parseEscaped(std::vector<std::string> &vec, const std::string &rawStr, const size_t &vecSize,
                             const int *insPos);
    static void quotedToStream(std::string& str,std::ofstream &stream);
};