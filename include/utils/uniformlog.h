#pragma once

#include <string>
#include "utils/colormod.h"

class UniformLog {

    std::string title;

public:
    static Color::Code curCode;
    explicit UniformLog(std::string title);
    UniformLog(const std::string& type,const std::string& title);
    ~UniformLog();
};