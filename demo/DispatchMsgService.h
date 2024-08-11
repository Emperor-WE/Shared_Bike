/****************************************
* 分发消息服务模块
    1.把外部收到的消息，转化成内部事件，也就是存在 data->msg->event 的解码过程
    2.解码后，在调用每个 event handler 来处理 event，
    3.每个 event handler 需要 subscribe 该 event 后才会被调用到。
****************************************/

#ifndef BRK_SERVICE_DISPATCH_EVENT_SERVICE_H_
#define BRK_SERVICE_DISPATCH_EVENT_SERVICE_H_

#include <map>
#include <vector>
#include <queue>
#include <vector>

#include "ievent.h"
#include "eventtype.h"
#include "iEventHandler.h"
#include "threadpool/thread_pool.h"
#include "NetworkInterface.h"

class DispatchMsgService
{
protected:
    DispatchMsgService();
public:
    
    virtual ~DispatchMsgService();

    virtual BOOL open();
    virtual void close();

    virtual void subscribe(u32 eid, iEventHandler* handler);
    virtual void unsubscribe(u32 eid, iEventHandler* handler);

    /*把事件投递到线程池做处理*/
    virtual i32 enqueue(iEvent* ev);
    /*对事件进行分发处理*/
    virtual std::vector<iEvent*> process(const iEvent* ev);
    //virtual iEvent* process(const iEvent* ev);

    /*线程池处理任务回调函数： 静态无this指针*/
    static void svc(void* argv);

    /*单例*/
    static DispatchMsgService* getInstance();

    iEvent* parseEvent(const char* message, u32 len, u32 eid);

    /*处理所有响应事件*/
    void handleAllResponseEvent(NetworkInterface* interface);  
protected:
    thread_pool_t* tp;
    static DispatchMsgService* DMS_;    /*单例对象*/

    typedef std::vector<iEventHandler*> T_EventHandlers;
    typedef std::map<u32, T_EventHandlers> T_EventHandlersMap;
    T_EventHandlersMap subscribers_;    /*绑定事件和处理者*/

    bool svr_exit_; /*状态 open/close*/

    static std::queue<iEvent*> response_events_;
    static pthread_mutex_t queue_mutex_;    /*用于锁住 response_events_*/
};

#endif