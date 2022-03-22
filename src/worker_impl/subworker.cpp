#include "worker_impl/subworker.h"
#include "structure/statement.h"
#include "utils/datparser.h"

SubWorker::SubWorker(Module *module, WorkDescriptor *workDes) : module(module), workDes(workDes), traceIndex(0) {

}

void SubWorker::work() {

    int stamp[2] = {0};
    for (int state = 0; state < workDes->stateCount; state++) {
        std::cout << "[SubWorker] " << workDes->db_name << "/" << workDes->table_name << " entering state " << state
                  << std::endl;
        if (state == 0) initialTrace();
        else alterTrace(state);
//        for (int source = 0; source < 2; source++) {
//            DATParser datParser(workDes->binlogPath / std::to_string(source) / (std::to_string(state) + ".dat"), source,
//                                stamp[source], workDes->table);
////            std::vector<Row *> newRows = std::move(datParser.parseData());
//
////            workDes->table.
////            std::cout << "Printing new rows" << std::endl;
////            for (const auto &row: newRows) {
////                for (const auto &col: row->data) {
////                    std::cout << col << ",";
////                }
////                std::cout << std::endl;
////            }
//        }
//        workDes->table->print(10);
    }
}

void SubWorker::initialTrace() {
    CreateTableStatement statement(workDes->binlogPath / "0" / "0.ddl", workDes->binlogPath / "0" / "0.ddlsql");
    workDes->table = new Table(workDes);
    statement.print();
    statement.fillToTable(workDes->table);
}

void SubWorker::alterTrace(int state) {
    AlterStatement *statement = AlterStatement::getAlterStatement(
            workDes->binlogPath / "0" / (std::to_string(state) + ".ddl"),
            workDes->binlogPath / "0" / (std::to_string(state) + ".ddlsql"));
    if (!statement) std::cout << "UNKNOWN ALTER!" << std::endl;
    else statement->print();

}
