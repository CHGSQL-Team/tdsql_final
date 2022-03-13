#include <iostream>
#include "config.h"
#include "module.h"
#include "binfetch.h"
#include "jdbc/cppconn/driver.h"

int main(int argc, char **argv) {
    Config config(argv);
    config.print_config();
    Module module(&config);
    BinaryFetcher binaryFetcher(&module);
    binaryFetcher.getBinary(0);
}
