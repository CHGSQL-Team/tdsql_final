#include "worker_impl/subworker.h"
#include "structure/statement.h"

SubWorker::SubWorker(Module *module, WorkDescriptor *workDes) : module(module), workDes(workDes), traceIndex(0) {

}

void SubWorker::work() {
    for (int i = 0; i < workDes->stateCount; i++) {
        std::cerr << "[SubWorker] " << workDes->db_name << "/" << workDes->table_name << " entering state " << i
                  << std::endl;
        if (i == 0) initialTrace();
        workDes->table->print(10);
    }
}

void SubWorker::initialTrace() {
    CreateTableStatement statement(workDes->binlogPath.append("/0/0.ddl"));
    workDes->table = new Table(workDes);
    statement.print();
    statement.fillToTable(workDes->table);
}
