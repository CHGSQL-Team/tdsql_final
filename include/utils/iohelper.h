#pragma once

#include <fstream>

class IOHelper{
public:
    std::ifstream* stream;
    explicit IOHelper(std::ifstream* stream);
    [[nodiscard]] std::string getLine() const;
    [[nodiscard]] int getLineInt() const;
    [[nodiscard]] std::string getEntireFile() const;
};