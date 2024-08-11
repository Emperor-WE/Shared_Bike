#ifndef NS_EVENT_H_
#define NS_EVENT_H_

#include "glo_def.h"
#include "eventtype.h"
#include <string>
#include <iostream>

/*请求事件实体*/
class iEvent
{
public:
    iEvent(u32 eid, u32 sn);

    virtual ~iEvent();
    virtual std::ostream& dump(std::ostream& out) const { return out; }
    //返回的是消息对象序列化后的字节大小（即消息对象的实际大小）。
    virtual i32 ByteSize() { return 0; }
    virtual bool SerializeToArray(char* buf, int len) { return TRUE; }

    //生成唯一的序列号
    u32 generateSeqNo();

    u32 get_eid() const { return eid_; }
    u32 get_sn() const { return sn_; }
    void set_eid(u32 eid) { eid_ = eid; }

    void set_args(void* args) { args_ = args; }
    void* get_args() { return args_; }

private:
    u32 eid_;   /*事件唯一编号*/
    u32 sn_;    /*序列号*/
    void* args_;    /*ConnectSession*/
};

#endif