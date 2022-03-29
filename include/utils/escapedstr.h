#pragma once

#include <vector>
#include <string>

class EscapedResolver {
public:
    static void parseEscaped(std::vector<std::string> &vec, std::string &rawStr, size_t vecSize);
    static void quotedToStream(std::string& str,std::ofstream &stream);
};