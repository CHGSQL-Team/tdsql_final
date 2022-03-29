#include "worker_impl/subworker.h"
#include "worker_impl/pusher.h"
#include "structure/statement.h"
#include "utils/datparser.h"
#include "utils/uniformlog.h"
#include "boost/asio/post.hpp"

SubWorker::SubWorker(Module *module, WorkDescriptor *workDes) : module(module), workDes(workDes) {

}

void SubWorker::work() {

    int stamp[2] = {0};
    for (int state = 0; state < workDes->stateCount; state++) {
        std::cout << "[SubWorker] " << workDes->db_name << "/" << workDes->table_name << " entering state " << state
                  << std::endl;
        Statement *statement = Statement::getStatement(workDes->binlogPath / "0" / (std::to_string(state) + ".ddl"),
                                                       workDes->binlogPath / "0" / (std::to_string(state) + ".ddlsql"));
        Trace(statement, state);
        for (int source = 0; source < 2; source++) {
            DATParser datParser(workDes->binlogPath / std::to_string(source) / (std::to_string(state) + ".dat"), source,
                                stamp[source], workDes->table);
            std::vector<Row *> newRows = std::move(datParser.parseData());
            workDes->table->insertRows(newRows);
        }
        if (module->config->enable_print_table)
            workDes->table->print(20);
    }
    workDes->table->optimizeTableForDump();
    workDes->table->dumpToFile(workDes->binlogPath / std::string("result.csv"));
    std::cout << "[Worker] Work for table " + workDes->db_name + "/" + workDes->table_name
              << " completed. Begin to push."
              << std::endl;
    if (!module->config->disable_pusher) {
        auto *pusher = new Pusher(workDes, module);
        Module *module_ = module;
        boost::asio::post(*module->pusherPool,
                          [=] {
                              pusher->push();
                              std::cout << "[Worker] Pushing table " + pusher->dbName + "/" + pusher->tableName
                                        << " completed." << std::endl;
                              module_->timed.printElapsedTime();
                              delete pusher;
                          });
    }

}

void SubWorker::Trace(Statement *statement_, int state) {
    if (dynamic_cast<CreateTableStatement *>(statement_)) {
        if (state != 0) {
            std::cout << "[SubWorker] Duplicated create table, refused to change." << std::endl;
            return;
        }
        workDes->table = new Table(workDes);
    }
    statement_->print();
    statement_->fillToTable(workDes->table);
}

SubWorker::~SubWorker() {
    delete workDes;
}
