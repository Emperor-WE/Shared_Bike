#ifndef _SHBK_COMMON_CONFIGDEF_H_
#define _SHBK_COMMON_CONFIGDEF_H_

#include <string>

typedef struct st_env_config
{
    std::string db_ip;
    unsigned short db_port;
    std::string db_user;
    std::string db_pwd;
    std::string db_name;

    unsigned short svr_port;

    st_env_config()
    {

    }

    st_env_config(const std::string& _db_ip, const unsigned short _db_port, const std::string& _db_user, 
        const std::string& _db_pwd, const std::string& _db_name, const unsigned short _svr_port)
    {
        db_ip = _db_ip;
        db_port = _db_port;
        db_user = _db_user;
        db_pwd = _db_pwd;
        db_name = _db_name;
        svr_port = _svr_port;
    }

    st_env_config& operator=(const st_env_config& config)
    {
        if(this != &config) {
            this->db_ip = config.db_ip;
            this->db_port = config.db_port;
            this->db_user = config.db_user;
            this->db_pwd = config.db_pwd;
            this->db_name = config.db_name;
            this->svr_port = config.svr_port;
        }

        return *this;
    }
}_st_env_config;

#endif