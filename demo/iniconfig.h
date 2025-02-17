#ifndef _SHBK_COMMON_INICONFIG_H_
#define _SHBK_COMMON_INICONFIG_H_

#include <string>
#include "configdef.h"

class Iniconfig
{
protected:
 Iniconfig();

public:
    ~Iniconfig();

    bool loadfile(const std::string& path);
    const _st_env_config& getconfig() const;

    static Iniconfig* getInstance();
private:
    _st_env_config _config;
    bool _isload;
    static Iniconfig* config;
};

#endif