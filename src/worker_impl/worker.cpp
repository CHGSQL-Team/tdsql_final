#include "worker_impl/worker.h"
#include "worker_impl/subworker.h"

Worker::Worker(Module *module) : module(module) {

}

void Worker::work() {
    for (auto const&[_, i]: module->works) {
        SubWorker subWorker(module, i);
        if(i->db_name=="a" && i->table_name=="a")
            subWorker.work();
    }
}
