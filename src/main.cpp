#include "config.h"
#include "structure/module.h"
#include "external/extfetcher.h"
#include "utils/workloader.h"
#include "worker_impl/worker.h"

int main(int argc, char **argv) {
    Config config(argv);
    Module module(&config);
    ExternalFetcher extFetcher(&module);
    extFetcher.evokeFetchAll(0, 0);
    WorkLoader workLoader(&module);
    workLoader.loadWorks();
    workLoader.printWorks();
    Worker worker(&module);
    worker.work();
}
