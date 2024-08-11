#ifndef BRKS_BUS_USER_HANDLER_H_
#define BRKS_BUS_USER_HANDLER_H_

#include "glo_def.h"
#include "iEventHandler.h"
#include "events_def.h"
#include "threadpool/thread.h"

#include <string>
#include <map>
#include <memory>

/*iEventHandler接口实现，用户事件处理*/
class UserEventHandler : public iEventHandler
{
public:
    UserEventHandler();
    virtual ~UserEventHandler();
    virtual iEvent* handle(const iEvent* ev);

private:
    /*手机验证码处理*/
    MobileCodeRspEv* handle_mobile_code_req(MobileCodeReqEv* ev);

    /*登录请求处理*/
    LoginResEv* handle_login_req(LoginReqEv* ev);

    /*产生验证码*/
    i32 code_gen(); 

private:
    //std::string mobile_;
    std::map<std::string, i32> m2c_;    /* first is mobile, second is code*/
    pthread_mutex_t pm_;
};

#endif