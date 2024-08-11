#include <iostream>
#include <unistd.h>
#include "bike.pb.h"
#include "event.h"
#include "events_def.h"
#include "user_event_handler.h"
#include "DispatchMsgService.h"
#include "NetworkInterface.h"
#include "iniconfig.h"
#include "Logger.h"
#include "sqlconnection.h"
#include "BusProcessor.h"

using namespace std;

int main(int argc, char** argv) {
    if(argc != 3) {
        fprintf(stderr, "Please input shbk <config file path> <log config file>\n");
        return -1;
    }

    if(!Logger::instance()->init(std::string(argv[2]))) {
        fprintf(stderr, "init log moudle failed.\n");
        return -2;
    }

    Iniconfig* config = Iniconfig::getInstance();
    if(!config->loadfile(std::string(argv[1]))) {
        //printf("load %s failed.\n", argv[1]);
        Logger::instance()->GetHandle()->error("load %s failed.", argv[1]);
        //LOG_ERROR("load %s failed.", argv[1]);
        return -3;
    }

    _st_env_config config_args = config->getconfig();
    LOG_INFO("database[ip:%s, port:%d, user:%s, pwd:%s, name:%s], server[port:%d]\n", config_args.db_ip.c_str(), config_args.db_port,
    config_args.db_user.c_str(), config_args.db_pwd.c_str(), config_args.db_name.c_str(), config_args.svr_port);

    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
    int ret = mysqlconn->Init(config_args.db_ip.c_str(), config_args.db_port, config_args.db_user.c_str(),
        config_args.db_pwd.c_str(), config_args.db_name.c_str());
    if(!ret)
    {
        LOG_ERROR("Database Init failed! exit.\n");
        return -4;
    }

    BusinessProcessor busPro(mysqlconn);
    busPro.init();
   
    DispatchMsgService* DMS = DispatchMsgService::getInstance();
    DMS->open();

    NetworkInterface* NTIF = new NetworkInterface();
    NTIF->start(config_args.db_port);

    while(1 == 1)
    {
        NTIF->network_event_dispatch();
        sleep(1);
        printf("etwork_event_dispatch...\n");
    }

}