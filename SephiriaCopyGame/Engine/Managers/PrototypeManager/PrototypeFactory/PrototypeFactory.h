#pragma once

#include <Engine/Object/Object/Object.h>


class PrototypeFactory  
{  
public:  
    static unique_ptr<Object> CreateFromPrototype(const string& prototypeName);  // ������Ÿ�� ��� ����
    static void CreateDefaultPrototypes();   // ������Ÿ�� �̸� ����
};