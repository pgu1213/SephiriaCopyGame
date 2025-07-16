#pragma once

#include <Engine/Object/Object/Object.h>


class PrototypeFactory  
{  
public:  
    static unique_ptr<Object> CreateFromPrototype(const string& prototypeName);  // 프로토타입 기반 생성
    static void CreateDefaultPrototypes();   // 프로토타입 미리 지정
};