#include "user_event_handler.h"
#include "DispatchMsgService.h"
#include "sqlconnection.h"
#include "iniconfig.h"
#include "user_service.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>

/*实现订阅事件的处理 --- 订阅者模式*/
UserEventHandler::UserEventHandler() : iEventHandler("UserEventHandler")
{
    DispatchMsgService::getInstance()->subscribe(EEVENTID_GET_MOBILE_CODE_REQ, this);
    thread_mutex_create(&pm_);
}

/*退订事件的处理*/
UserEventHandler::~UserEventHandler()
{
    DispatchMsgService::getInstance()->unsubscribe(EEVENTID_GET_MOBILE_CODE_REQ, this);
    thread_mutex_destroy(&pm_);
}

/* 依据传入的事件类型，分别做出不同的处理 */
iEvent* UserEventHandler::handle(const iEvent* ev)
{
    if(ev == NULL)
    {
        //LOG_ERROR("input ev is NULL");
        printf("input ev is NULL");
    }

    u32 eid = ev->get_eid();

    if(eid == EEVENTID_GET_MOBILE_CODE_REQ)
    {
        return  handle_mobile_code_req((MobileCodeReqEv*)ev);
    }
    else if(eid == EEVENTID_LOGIN_REQ)
    {
        return handle_login_req((LoginReqEv*)ev);
    }
    else if(eid == EEVENTID_RECHARGE_REQ)
    {
        //return handle_recharge_req((RechargeEv*)ev);
    }
    else if(eid == EEVENTID_GET_ACCOUNT_BALANCE_REQ)
    {
        //return handle_get_account_balance_req((GetAccountBalanceEv*)ev);
    }
    else if(eid == EEVENTID_LIST_ACCOUNT_RECORDS_REQ)
    {
        //return handle_list_account_records_req((ListAccountRecordsReqEv*)ev);
    }

    return NULL;
}

MobileCodeRspEv* UserEventHandler::handle_mobile_code_req(MobileCodeReqEv* ev)
{
    i32 icode = 0;
    std::string mobile_ = ev->get_mobile(); /*获取手机号码*/
    //LOG_DEBUG("try to get mobile phone %s vaildate code.", mobile_.c_str());
    printf("try to get mobile phone %s vaildate code.\n", mobile_.c_str());

    icode = code_gen(); /*生成对应手机验证码*/

    thread_mutex_lock(&pm_);
    m2c_[mobile_] = icode;  /*[手机号码 ： 验证码]*/
    thread_mutex_unlock(&pm_);

    printf("mobile: %s, code: %d\n", mobile_.c_str(), icode);

    //LOG_DEBUG("send sms success");
    return new MobileCodeRspEv(200, icode);
}

 LoginResEv* UserEventHandler::handle_login_req(LoginReqEv* ev)
 {
    LoginResEv* loginEv = nullptr;
    std::string mobile = ev->get_mobile();
    i32 code = ev->get_icode();

    LOG_DEBUG("try to handle login ev, mobile = %s, code = %d\n", mobile.c_str(), code);

    thread_mutex_lock(&pm_);
    auto iter = m2c_.find(mobile);

    if( ((iter != m2c_.end()) && (code != iter->second)) || (iter == m2c_.end()))
    {
        loginEv =  new LoginResEv(ERRC_INVALID_DATA);
    }
    thread_mutex_unlock(&pm_);

    if(loginEv) return loginEv;

    /*验证成功：则判断用户数据是否存在，不存在则插入记录*/
    std::shared_ptr<MysqlConnection> mysqlconn(new MysqlConnection);
    _st_env_config config_args = Iniconfig::getInstance()->getconfig();

    int ret = mysqlconn->Init(config_args.db_ip.c_str(), config_args.db_port, config_args.db_user.c_str(),
        config_args.db_pwd.c_str(), config_args.db_name.c_str());
    if(!ret)
    {
        LOG_ERROR("UserEventHandler::handle_login_req - Database Init failed!\n");
        return new LoginResEv(ERRO_PROCCESS_FAILED);
    }

    UserService us(mysqlconn);
    bool result = false;

    if(!us.exist(mobile))
    {
        result = us.insert(mobile);
        if(!result)
        {
            LOG_ERROR("insert user(%s) to db failed.\n", mobile.c_str());
            return new LoginResEv(ERRO_PROCCESS_FAILED);
        }
    }

    return new LoginResEv(ERRC_SUCCESS);
 }

i32 UserEventHandler::code_gen()
{
    i32 code = 0;
    srand((unsigned int)time(NULL));

    code = (unsigned int)(rand() % (9999999 - 100000) + 100000);

    return code;
}