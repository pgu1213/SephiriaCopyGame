#pragma once
#include "../SingletonManager/SingletonManager.h"

class ConfigManager : public SingleTon<ConfigManager>
{
    friend class SingleTon<ConfigManager>;
private:
    explicit ConfigManager();
public:
    virtual ~ConfigManager();
public:

};