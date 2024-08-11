#ifndef NS_IEVENT_HANDLER_H_
#define NS_IEVENT_HANDLER_H_

#include "ievent.h"
#include "eventtype.h"

/*接口：负责处理相关的事件(如：用户、单车事件等)*/
class iEventHandler
{
public:
    virtual iEvent* handle(const iEvent* ev) { return NULL; }
    virtual ~iEventHandler() { }

    iEventHandler(const std::string& name) : name_(name) { }

    std::string& get_name() { return name_; }
private:
    std::string name_;
};

#endif