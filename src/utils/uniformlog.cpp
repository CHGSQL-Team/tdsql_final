#include "utils/uniformlog.h"
#include "utils/colormod.h"

#include <utility>
#include <iostream>

Color::Code UniformLog::curCode = Color::Code::FG_BLUE;

UniformLog::UniformLog(std::string title) : title(std::move(title)) {

    if (curCode == Color::Code::FG_BLUE) curCode = Color::Code::FG_GREEN;
    else if (curCode == Color::Code::FG_GREEN) curCode = Color::Code::FG_RED;
    else if (curCode == Color::Code::FG_RED) curCode = Color::Code::FG_BLUE;

    Color::Modifier modClr(curCode);

    std::cout << modClr << "[" << this->title << "] Log " << std::endl;
}

UniformLog::~UniformLog() {
    Color::Modifier def(Color::Code::FG_DEFAULT);
    std::cout << def << std::endl;

}

UniformLog::UniformLog(const std::string &type, const std::string &title) : UniformLog("[" + type + "] " + title) {

}
