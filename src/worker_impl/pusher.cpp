#include "worker_impl/pusher.h"

Pusher::Pusher(WorkDescriptor *workDes, Module *module) : workDes(workDes), module(module) {

}

void Pusher::push() {
    SQLInstance *instance = module->sqlPool->getSQLInstance();

}
