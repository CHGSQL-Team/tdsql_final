#include "worker_impl/subworker.h"
#include "statement.h"

SubWorker::SubWorker(Module *module, WorkDescriptor *workDes) : module(module), workDes(workDes), traceIndex(0) {

}

void SubWorker::work() {
    for (int i = 0; i < workDes->stateCount; i++) {
        std::cerr << "[SB] " << workDes->db_name << "/" << workDes->table_name << " entering state " << i << std::endl;
    }
}

void SubWorker::initialTrace() {
    CreateTableStatement statement(workDes->binlogPath.append("/0/0.ddl"));
    statement.print();
}
