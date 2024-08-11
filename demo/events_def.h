#ifndef BRKS_COMMON_EVENTS_DEF_H_
#define BRKS_COMMON_EVENTS_DEF_H_

#include "ievent.h"
#include <string>
#include "bike.pb.h"

/*短信响应*/
class MobileCodeReqEv : public iEvent
{
public:
    MobileCodeReqEv(const std::string& mobile) : iEvent(EEVENTID_GET_MOBILE_CODE_REQ, iEvent::generateSeqNo())
    {
        msg_.set_mobile(mobile);
    }

    const std::string get_mobile() const
    {
        return msg_.mobile();
    }

    virtual std::ostream& dump(std::ostream& out) const ;
    // 返回的是消息对象序列化后的字节大小（即消息对象的实际大小）。
    virtual i32 ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
private:
    tutorial::mobile_request msg_;  /* mobile:手机号码*/
};

/*短信请求*/
class MobileCodeRspEv : public iEvent
{
public:
    MobileCodeRspEv(i32 code, i32 icode) : iEvent(EEVENTID_GET_MOBILE_CODE_RSP, iEvent::generateSeqNo())
    {
        msg_.set_code(code);
        msg_.set_icode(icode);
        msg_.set_data(getReasonByErrorCode(code));
    }

    const i32 get_code() { return msg_.code(); }
    const i32 get_icode() { return msg_.icode(); }
    const std::string& get_data() { return msg_.data(); }

    virtual std::ostream& dump(std::ostream& out) const;
    virtual i32 ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }

private:
    tutorial::mobile_response msg_; /* code:响应代号 icode:验证码 data:失败信息*/
};

class ExitRspEv : public iEvent
{
public:
    ExitRspEv() : iEvent(EEVENTID_EXIT_RSP, iEvent::generateSeqNo())
    {

    }
};

class LoginReqEv : public iEvent
{
public:
    LoginReqEv(const std::string& mobile, i32 icode)
        : iEvent(EEVENTID_LOGIN_REQ, iEvent::generateSeqNo())
    {
        msg_.set_mobile(mobile);
        msg_.set_icode(icode);
    }

    const std::string& get_mobile() { return msg_.mobile(); }
    const i32 get_icode() { return msg_.icode(); }

    virtual std::ostream& dump(std::ostream& out) const;
    virtual i32 ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
    
private:
    tutorial::login_request msg_;
};

class LoginResEv : public iEvent
{
public:
    LoginResEv(i32 code)
        : iEvent(EEVENTID_LOGIN_RSP, iEvent::generateSeqNo())
    {
        msg_.set_code(code);
        msg_.set_desc(getReasonByErrorCode(code));
    }

    const i32 get_code() { return msg_.code(); }
    const std::string get_desc() { return msg_.desc(); }

    virtual std::ostream& dump(std::ostream& out) const;
    virtual i32 ByteSize() { return msg_.ByteSizeLong(); }
    virtual bool SerializeToArray(char* buf, int len) { return msg_.SerializeToArray(buf, len); }
private:
    tutorial::login_response msg_;
};


#endif
