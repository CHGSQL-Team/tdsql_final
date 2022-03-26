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

        if ((i->db_name == "a" || i->db_name == "b" || i->db_name == "c") &&
            (i->table_name == "a" || i->table_name == "b" || i->table_name == "c")) {
            std::cout << "[Worker] Begin to work table " + i->db_name + "/" + i->table_name << std::endl;
            module->timed.printElapsedTime();
            subWorker->work();
            std::cout << "[Worker] Work for table " + i->db_name + "/" + i->table_name << " completed. Begin to push."
                      << std::endl;
            module->timed.printElapsedTime();
            auto *pusher = new Pusher(workDes, module);
            delete subWorker;
            boost::asio::post(*module->pusherPool,
                              [=] {
                                  pusher->push();
                                  std::cout << "[Worker] Pushing table " + pusher->dbName + "/" + pusher->tableName
                                            << " completed." << std::endl;
                                  module->timed.printElapsedTime();
                                  delete pusher;
                              });
        }
    }
}

Worker::~Worker() {
    delete module->pusherPool;
    module->pusherPool = nullptr;
}
