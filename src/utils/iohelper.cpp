#include "utils/iohelper.h"
#include "boost/lexical_cast.hpp"

IOHelper::IOHelper(std::ifstream *stream) : stream(stream) {
}

std::string IOHelper::getLine() const {
    std::string ret;
    std::getline(*stream, ret);
    return ret;
}

int IOHelper::getLineInt() const {
    std::string tmp;
    std::getline(*stream, tmp);
    return boost::lexical_cast<int>(tmp);
}

std::string IOHelper::getEntireFile() const {
    std::ostringstream ss;
    ss << stream->rdbuf();
    return ss.str();
}
