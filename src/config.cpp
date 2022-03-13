#include "config.h"
#include <iostream>

Config::Config(char **argv) {
    sql_ip[0] = std::string(argv[1]);
    sql_port[0] = std::stoi(std::string(argv[2]));
    sql_usr[0] = std::string(argv[3]);
    sql_pwd[0] = std::string(argv[4]);
    sql_gtid[0] = std::string(argv[5]);
    sql_ip[1] = std::string(argv[6]);
    sql_port[1] = std::stoi(std::string(argv[7]));
    sql_usr[1] = std::string(argv[8]);
    sql_pwd[1] = std::string(argv[9]);
    sql_gtid[1] = std::string(argv[10]);
    sql_ip[2] = std::string(argv[11]);
    sql_port[2] = std::stoi(std::string(argv[12]));
    sql_usr[2] = std::string(argv[13]);
    sql_pwd[2] = std::string(argv[14]);
}

void Config::print_config() const {
    std::cerr << "Printing configuration ---" << std::endl;
    for (int i = 0; i < 3; i++) {
        std::cerr << "IP " << i << " is " << sql_ip[i] << std::endl;
        std::cerr << "Port " << i << " is " << sql_port[i] << std::endl;
        std::cerr << "Username " << i << " is " << sql_usr[i] << std::endl;
        std::cerr << "Password " << i << " is " << sql_pwd[i] << std::endl;
        if (i != 2) std::cerr << "GTID position " << i << " is " << sql_gtid[i] << std::endl;
    }
    std::cerr << "Configuration ends here ---" << std::endl;
}
