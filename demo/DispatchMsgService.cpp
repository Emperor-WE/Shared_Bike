#include "DispatchMsgService.h"
#include "bike.pb.h"
#include "events_def.h"

#include <algorithm>

DispatchMsgService* DispatchMsgService::DMS_ = new DispatchMsgService();
pthread_mutex_t DispatchMsgService::queue_mutex_;
std::queue<iEvent*> DispatchMsgService::response_events_;

DispatchMsgService::DispatchMsgService()
    : tp(NULL)
{

}

DispatchMsgService::~DispatchMsgService()
{
    if(svr_exit_ == FALSE)
    {
        close();
    }
}

/* 初始化线程池 */
BOOL DispatchMsgService::open()
{
    svr_exit_ = FALSE;
    thread_mutex_create(&queue_mutex_);

    tp = thread_pool_init();

    return tp ? TRUE : FALSE;
}

/* 销毁线程池 */
void DispatchMsgService::close()
{
    svr_exit_ = TRUE;
    thread_pool_destroy(tp);
    thread_mutex_destroy(&queue_mutex_);
    subscribers_.clear();

    tp = NULL;
}

/* 将处理事件添加到 subscribe_ 集合之中 */
void DispatchMsgService::subscribe(u32 eid, iEventHandler* handler)
{
    LOG_DEBUG("DispatchMsgService::subscribe eid:%u\n", eid);
    T_EventHandlersMap::iterator iter = subscribers_.find(eid);
    if(iter != subscribers_.end())
    {
        T_EventHandlers::iterator hdl_iter = std::find(iter->second.begin(), iter->second.end(), handler);
        if(hdl_iter == iter->second.end())
        {
            iter->second.push_back(handler);
        }
    }
    else
    {
        subscribers_[eid].push_back(handler);
    }
}

/* 取消订阅 */
void DispatchMsgService::unsubscribe(u32 eid, iEventHandler* handler)
{
    T_EventHandlersMap::iterator iter = subscribers_.find(eid);
    if(iter != subscribers_.end())
    {
        T_EventHandlers::iterator hdl_iter = std::find(iter->second.begin(), iter->second.end(), handler);
        if(hdl_iter != iter->second.end())
        {
            iter->second.erase(hdl_iter);
        }
    }
}

/* 把事件投递到线程池做处理，设置回调函数为 svc */
i32 DispatchMsgService::enqueue(iEvent* ev)
{
    if(NULL == ev)
    {
        return -1;
    }

    thread_task_t* task = thread_task_alloc(0);

    task->handler = DispatchMsgService::svc;
    task->ctx = (void*)ev;

    return thread_task_post(tp, task);
}

/* 对事件进行分发处理，将ev->eid 编号的事件，全部处理，将响应事件添加到res_events中 */
std::vector<iEvent*> DispatchMsgService::process(const iEvent* ev)
//iEvent* DispatchMsgService::process(const iEvent* ev)
{
    LOG_DEBUG("DispatchMsgService::process -ev:%p\n", ev);
    if(NULL == ev)
    {
        return {};
    }

    u32 eid = ev->get_eid();    //获取事件的唯一编号

    LOG_DEBUG("DispatchMsgService::process -eid:%u\n", eid);

    if(eid == EEVENTID_UNKOWN) 
    {
        LOG_WARN("DispatchMsgService unknow event:%u\n", eid);
        return {};
    }

    //LOG_DEBUG("dispatch ev:%d\n", ev->get_eid());

    T_EventHandlersMap::iterator handlers = subscribers_.find(eid);
    if(handlers == subscribers_.end())
    {
        LOG_WARN("DispatchMsgService : no any handler subscribed %d", eid);
        return {};
    }

    /*
    iEvent* rsp = NULL;
    for(auto iter = handlers->second.begin(); iter != handlers->second.end(); iter++)
    {
        iEventHandler* handler = *iter;
        LOG_DEBUG("DispatchMsgService : get handler:%s\n", handler->get_name().c_str());

        rsp = handler->handle(ev);
        if(rsp == NULL)
        {
            LOG_ERROR("DispatchMsgService::process - handle event eid=%d error\n", ev->get_eid());
            continue;
        }
    }

    return rsp;
    */
    iEvent* rsp = NULL;
    std::vector<iEvent*> rsp_events;
    for(auto iter = handlers->second.begin(); iter != handlers->second.end(); iter++)
    {
        iEventHandler* handler = *iter;
        LOG_DEBUG("DispatchMsgService : get handler:%s\n", handler->get_name().c_str());

        rsp = handler->handle(ev);
        if(rsp == NULL)
        {
            LOG_ERROR("DispatchMsgService::process - handle event eid=%d error\n", ev->get_eid());
            continue;
        }
        rsp_events.push_back(rsp);
    }

    return rsp_events;
    
}

/*
* 线程池处理任务回调函数： 静态无this指针
* 将res_events 中每个事件拿出来，设置参数args，并将其添加到 response_events_中
*/
void DispatchMsgService::svc(void* argv)
{
    DispatchMsgService* dms = DispatchMsgService::getInstance();
    iEvent* ev = (iEvent*)argv;

    if(!dms->svr_exit_)
    {
        LOG_DEBUG("DispatchMsgService::svc...\n");
        //iEvent* rsp = dms->process(ev);
        auto rsp_events = dms->process(ev);
        for(auto rsp : rsp_events)
        {
            if(rsp)
            {
                rsp->dump(std::cout);
                rsp->set_args(ev->get_args());  /*args: ConnectSession* cs */
            }
            else
            {
                /*生成终止响应事件*/
               rsp = new ExitRspEv();
               rsp->set_args(ev->get_args());
            }

            thread_mutex_lock(&queue_mutex_);
            response_events_.push(rsp);
            thread_mutex_unlock(&queue_mutex_);
        }
        //delete ev;  /*删除事件这里需要斟酌*/
    }
}

/*单例*/
DispatchMsgService* DispatchMsgService::getInstance()
{
    if(DMS_ == nullptr)
    {
        DMS_ = new DispatchMsgService();
    }

    return DMS_;
}

/* 依据传入的事件编号 eid，将 message 按照特定的事件类型解析 */
iEvent* DispatchMsgService::parseEvent(const char* message, u32 len, u32 eid)
{
    if(!message)
    {
        LOG_ERROR("DispatchMsgService::parseEvent - message is null[eid:%d].\n", eid);
        return nullptr;
    }

    if(eid == EEVENTID_GET_MOBILE_CODE_REQ)
    {
        tutorial::mobile_request mr;
        if(mr.ParseFromArray(message, len))
        {
            MobileCodeReqEv* ev = new MobileCodeReqEv(mr.mobile());
            return ev;
        }
    }
    else if(eid == EEVENTID_LOGIN_REQ)
    {
        tutorial::login_request lr;
        if(lr.ParseFromArray(message, len))
        {
            LoginReqEv* ev = new LoginReqEv(lr.mobile(), lr.icode());
            return ev;
        }
    }
    return nullptr;
}

/* 对于 response_events_ 中的事件依据事件类型 逐一处理，获取事件的时候加锁处理 */
void DispatchMsgService::handleAllResponseEvent(NetworkInterface* interface)
{
    bool done = false;

    while(!done)
    {
        iEvent* ev = nullptr;

        thread_mutex_lock(&queue_mutex_);
        if(!response_events_.empty())
        {
            ev = response_events_.front();
            response_events_.pop();
        }
        else
        {
            done = true;
        }
        thread_mutex_unlock(&queue_mutex_);
        
        if(!done)
        {
            if(ev->get_eid() == EEVENTID_GET_MOBILE_CODE_RSP)
            {
                //MobileCodeRspEv* mcre = static_cast<MobileCodeRspEv*>(ev);
                LOG_DEBUG("DispatchMsgService::handleAllResponseEvent - id:EEVENTID_GET_MOBILE_CODE_RSP\n");

                ConnectSession* cs = (ConnectSession*)(ev->get_args());
                cs->response = ev;

                /*序列化请求数据*/
                cs->message_len = ev->ByteSize();
                cs->write_buf = new char[cs->message_len + MESSAGE_HEADER_LEN];

                /*组装头部*/
                memcpy(cs->write_buf, MESSAGE_HEADER_ID, 4);
                *(u16*)(cs->write_buf + 4) = EEVENTID_GET_MOBILE_CODE_RSP;
                *(i32*)(cs->write_buf + 6) = cs->message_len;

                ev->SerializeToArray(cs->write_buf + MESSAGE_HEADER_LEN, cs->message_len);

                interface->send_response_message(cs);
            }
            else if(ev->get_eid() == EEVENTID_LOGIN_RSP)
            {
                //LoginResEv* lgre = static_cast<LoginResEv*>(ev);
                LOG_DEBUG("DispatchMsgService::handleAllResponseEvent - id:EEVENTID_LOGIN_RSP\n");

                ConnectSession* cs = (ConnectSession*)(ev->get_args());
                cs->response = ev;

                /*序列化请求数据*/
                cs->message_len = ev->ByteSize();
                cs->write_buf = new char[cs->message_len + MESSAGE_HEADER_LEN];

                /*组装头部*/
                memcpy(cs->write_buf, MESSAGE_HEADER_ID, 4);
                *(u16*)(cs->write_buf + 4) = EEVENTID_LOGIN_RSP;
                *(i32*)(cs->write_buf + 6) = cs->message_len;

                ev->SerializeToArray(cs->write_buf + MESSAGE_HEADER_LEN, cs->message_len);

                interface->send_response_message(cs);
            }
            else if(ev->get_eid() == EEVENTID_EXIT_RSP)
            {
                ConnectSession* cs = (ConnectSession*)(ev->get_args());
                cs->response = ev;
                interface->send_response_message(cs);
            }
        }
    }
}