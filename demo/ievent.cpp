#include "ievent.h"

iEvent::iEvent(u32 eid, u32 sn)
    : eid_(eid), sn_(sn)
{

}

iEvent::~iEvent()
{

}/* 生成唯一的序列号 */
u32 iEvent::generateSeqNo()
{
    static u32 sn = 0;
    return sn++;
}