#include "worker_impl/worker.h"
#include "worker_impl/subworker.h"

Worker::Worker(Module *module) : module(module) {

}

void Worker::work() {
    for (auto const&[_, i]: module->works) {
        SubWorker subWorker(module, i);
        subWorker.work();
    }
}
