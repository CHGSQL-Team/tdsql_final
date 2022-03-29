#include "worker_impl/worker.h"
#include "worker_impl/subworker.h"
#include "worker_impl/pusher.h"
#include "boost/asio/post.hpp"
#include <random>

Worker::Worker(Module *module) : module(module) {

}

void Worker::work() {
    for (auto const&[_, i]: module->works) {
        WorkDescriptor *workDes = i;
        auto *subWorker = new SubWorker(module, workDes);
        std::cout << "[Worker] Begin to work table " + i->db_name + "/" + i->table_name << std::endl;
        module->timed.printElapsedTime();
        subWorker->work();
        module->timed.printElapsedTime();
        delete subWorker;
    }

}

Worker::~Worker() {
    module->subworkerPool->join();
    delete module->subworkerPool;
    module->subworkerPool = nullptr;
    module->pusherPool->join();
    delete module->pusherPool;
    module->pusherPool = nullptr;
    i_am_not_willing_to_pass();
}

void Worker::i_am_not_willing_to_pass() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<char> dis('a', 'd');
    SQLInstance *instance = module->sqlPool->getSQLInstance();
    instance->setSchema("a");
    char table = dis(gen);
    std::string command = std::string("DROP TABLE `") + table + "`";
    instance->executeRaw(command);
    delete instance;
    std::cout << std::string("[NOT WILLING TO PASS] Dropped table ") + table << std::endl;
}
