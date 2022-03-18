#pragma once

#include <string>

class Config {
public:
    std::string sql_ip[3];
    unsigned int sql_port[3];
    std::string sql_usr[3];
    std::string sql_pwd[3];
    std::string sql_gtid[2];
    std::string log_path = "./logs/";
    std::string tmp_path = "./tmp/";
    std::string binlog_path = "./tmp/binlogs/";
    std::size_t seed = 2022;
    std::string class_path = "../extfetcher/build/production/extfetcher";
    std::string connector_jar_path = "../extfetcher/libs/mysql-binlog-connector-java.jar";
    std::string lang3_jar_path = "../extfetcher/libs/commons-lang3-3.12.0.jar";

    int hash_split_threshold = 4000000;
    int push_split_threshold = 10000;
    int subworker_threads = 2; //should be set to 2 now
    int hasher_threads = 12;
    int pusher_threads = 40;
    int reducer_threads = 12;
    int primary_key_recovery_threads = 5;

    bool console_show_sql_file = false;
    bool console_show_table_info = false;
    bool drop_all_primary_keys = false; // speed up insert
    bool try_use_shared_key = false;
    bool compress_row = false;
    bool use_transation = false;
    bool try_use_myisam_engine = false;


    void print_config() const;

    explicit Config(char **argv);

};