#include "config.h"
#include "module.h"
#include "extfetcher.h"
#include "workloader.h"

int main(int argc, char **argv) {
    Config config(argv);
    Module module(&config);
    ExternalFetcher extFetcher(&module);
    extFetcher.evokeFetchAll(0, 0);
    WorkLoader workLoader(&module);
    workLoader.loadWorks();
    workLoader.printWorks();
}
