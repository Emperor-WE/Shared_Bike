#include "BusProcessor.h"
#include "SqlTables.h"

BusinessProcessor::BusinessProcessor(std::shared_ptr<MysqlConnection> conn) 
    : mysqlconn_(conn), ueh_(new UserEventHandler)
{

}

BusinessProcessor::~BusinessProcessor()
{
    ueh_.reset();
}
    
bool BusinessProcessor::init()
{
    SqlTables tables(mysqlconn_);
    tables.CreateUserInfo();
    tables.CreateBikeTable();

    return true;
}