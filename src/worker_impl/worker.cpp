#include "worker_impl/worker.h"
#include "worker_impl/subworker.h"
#include "worker_impl/pusher.h"

Worker::Worker(Module *module) : module(module) {

}

void Worker::work() {
    for (auto const&[_, i]: module->works) {
        SubWorker subWorker(module, i);
        Pusher pusher(i, module);
        if ((i->db_name == "a" || i->db_name == "b" || i->db_name == "c") &&
            (i->table_name == "a" || i->table_name == "b" || i->table_name == "c")) {
//        if (i->db_name == "a" && i->table_name == "d") {
            subWorker.work();
            pusher.push();
        }
        module->timed.printElapsedTime();

    }
}
