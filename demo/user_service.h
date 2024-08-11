#ifndef BRKS_SVR_USER_SERVICE_H_
#define BRKS_SVR_USER_SERVICE_H_

#include "sqlconnection.h"

#include <memory>

class UserService
{
public:
    UserService(std::shared_ptr<MysqlConnection> sql_conn);

    bool exist(const std::string& mobile);
    bool insert(const std::string& mobile);
private:
    std::shared_ptr<MysqlConnection> sql_conn_;
}; 

#endif