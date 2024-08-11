#include "events_def.h"
#include <sstream>

std::ostream& MobileCodeReqEv::dump(std::ostream& out) const
{
    out << "MobileCodeReqEv sn=" << get_sn() << ",";
    out << "mobile=" << msg_.mobile() << std::endl;

    return out;
}

std::ostream& MobileCodeRspEv::dump(std::ostream& out) const
{
    out << "MobileCodeRspEv sn=" << get_sn() << ",";
    out << "code=" << msg_.code() << std::endl;
    out << "icode=" << msg_.icode() << std::endl;
    out << "describle = " << msg_.data() << std::endl;

    return out;
}

std::ostream& LoginReqEv::dump(std::ostream& out) const
{
    out << "LoginReqEv sn=" << get_sn() << ",";
    out << "mobile=" << msg_.mobile() << std::endl;
    out << "icode=" << msg_.icode() << std::endl;

    return out;
}

std::ostream& LoginResEv::dump(std::ostream& out) const
{
    out << "LoginResEv sn=" << get_sn() << ",";
    out << "code=" << msg_.code() << std::endl;
    out << "describle = " << msg_.desc() << std::endl;

    return out;
}
