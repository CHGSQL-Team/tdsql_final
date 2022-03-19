#include <fstream>

class IOHelper{
public:
    std::ifstream* stream;
    explicit IOHelper(std::ifstream* stream);
    std::string getLine() const;
    int getLineInt() const;
};