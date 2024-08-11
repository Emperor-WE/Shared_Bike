#include <iostream>
#include "iniconfig.h"
#include "configdef.h"
#include "Logger.h"
//#include "common/iniconfig.h"
//#include "common/configdef.h"

/**/
int main(int argc, char* argv[]) {
    if(argc != 3) {
        fprintf(stderr, "Please input shbk <config file path> <log config file>\n");
        return -1;
    }

    if(!Logger::instance()->init(std::string(argv[2]))) {
        fprintf(stderr, "init log moudle failed.\n");
        return -2;
    }

    Iniconfig config;
    if(!config.loadfile(std::string(argv[1]))) {
        //printf("load %s failed.\n", argv[1]);
        //LOG_ERROR Logger::instance()->GetHandle()->error("load %s failed.", argv[1]);
        LOG_ERROR("load %s failed.", argv[1]);
        return -3;
    }

    _st_env_config config_args = config.getconfig();
    LOG_INFO("database[ip:%s, port:%d, user:%s, pwd:%s, name:%s], server[port:%d]\n", config_args.db_ip.c_str(), config_args.db_port,
    config_args.db_user.c_str(), config_args.db_pwd.c_str(), config_args.db_name.c_str(), config_args.svr_port);
}