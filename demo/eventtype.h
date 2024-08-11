#ifndef BRKS_COMMON_EVENT_TYPE_H_
#define BRKS_COMMON_EVENT_TYPE_H_

#include "glo_def.h"

typedef struct EErrorReason_
{
    i32 code;
    const char* reason;
}EErrorReason;

/*事件ID*/
enum EventID
{
    EEVENTID_GET_MOBILE_CODE_REQ    = 0x01,
    EEVENTID_GET_MOBILE_CODE_RSP    = 0x02,

    EEVENTID_LOGIN_REQ              = 0x03,
    EEVENTID_LOGIN_RSP              = 0x04,

    EEVENTID_RECHARGE_REQ           = 0x5,
    EEVENTID_RECHARGE_RSP           = 0x6,

    EEVENTID_GET_ACCOUNT_BALANCE_REQ    = 0x07,
    EEVENTID_ACCOUNT_ABLANCE_RSP        = 0x08,

    EEVENTID_LIST_ACCOUNT_RECORDS_REQ   = 0x09,
    EEVENTID_ACCOUNT_RECORDS_RSP        = 0x10,

    EEVENTID_LIST_TRAVELS_REQ           = 0x11,
    EEVENTID_LIST_TRAVELS_RSP           = 0x12,

    EEVENTID_EXIT_RSP                   = 0xFE, /*无需回复响应 --- 释放资源即可*/
    EEVENTID_UNKOWN                     = 0xFF
};

/*事件响应错误代号*/
enum EErrorCode
{
    ERRC_SUCCESS            = 200,
    ERRC_INVALID_MSG        = 400,      /*无效的消息*/
    ERRC_INVALID_DATA       = 404,      /*无效的数据*/
    ERRC_METHOD_NOT_ALLOWED = 405,      /*方法不允许*/
    ERRO_PROCCESS_FAILED    = 406,      /*服务器端处理失败*/
    ERRO_BIKE_IS_TOOK       = 407,      
    ERRO_BIKE_IS_RUNNING    = 408,
    ERRO_BIKE_IS_DAMAGED    = 409,       /*单车损坏*/
    ERR_NULL                = 0
};

/*依据 code 查表 错误描述*/
const char* getReasonByErrorCode(i32 code);

#endif