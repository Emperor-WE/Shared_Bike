#include "iniconfig.h"
//#include "../../third/include/iniparser/iniparser.h"
#include <iniparser/iniparser.h>

Iniconfig* Iniconfig::config = nullptr;

Iniconfig::Iniconfig() : _isload(false) {

}

Iniconfig::~Iniconfig() {

}

bool Iniconfig::loadfile(const std::string& path) {
    dictionary* ini = NULL;

    if(!_isload) {
        ini = iniparser_load(path.c_str());
        if(NULL == ini) {
            fprintf(stderr, "cannot parse file:%s\n", path.c_str());
            return false;
        }

        const char* ip = iniparser_getstring(ini, "database:ip", "127.0.0.1");
        int db_port = iniparser_getint(ini, "database:port", 3306);
        const char* user = iniparser_getstring(ini, "database:user", "root");
        const char* pwd = iniparser_getstring(ini, "database:pwd", "12345678");
        const char* name = iniparser_getstring(ini, "database:name", "qiniubike");
        int svr_port = iniparser_getint(ini, "server:port", 9090);

        _config = st_env_config(std::string(ip), db_port, std::string(user), std::string(pwd), 
            std::string(name), svr_port);

        iniparser_freedict(ini);
        
        _isload = true;
    }

    return _isload;
}

Iniconfig* Iniconfig::getInstance()
{
    if(config == nullptr)
    {
        return new Iniconfig();
    }

    return config;
}

const _st_env_config& Iniconfig::getconfig() const {
    return _config;
}
