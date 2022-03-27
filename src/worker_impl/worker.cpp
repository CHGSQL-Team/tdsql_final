#include "worker_impl/worker.h"
#include "worker_impl/subworker.h"
#include "worker_impl/pusher.h"
#include "boost/asio/post.hpp"

Worker::Worker(Module *module) : module(module) {

}

void Worker::work() {
    for (auto const&[_, i]: module->works) {
        WorkDescriptor *workDes = i;
        auto *subWorker = new SubWorker(module, workDes);

        // We are really PASSING this !!!!
//        if (!(i->table_name == "a" || i->table_name == "b" || i->table_name == "c" || i->table_name == "d")) {
            std::cout << "[Worker] Begin to work table " + i->db_name + "/" + i->table_name << std::endl;
            module->timed.printElapsedTime();
            subWorker->work();
            module->timed.printElapsedTime();
            delete subWorker;
//            boost::asio::post(*module->subworkerPool, [=] {
//                subWorker->work();
//                module->timed.printElapsedTime();
//                delete subWorker;
//            });
//        }
    }
}

Worker::~Worker() {
    module->subworkerPool->join();
    delete module->subworkerPool;
    module->subworkerPool = nullptr;
    module->pusherPool->join();
    delete module->pusherPool;
    module->pusherPool = nullptr;
}
