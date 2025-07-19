#pragma once
#include "../SingletonManager/SingletonManager.h"

class FileManager : public SingleTon<FileManager>
{
    friend class SingleTon<FileManager>;
private:
    explicit FileManager();
public:
    virtual ~FileManager();
public:

};