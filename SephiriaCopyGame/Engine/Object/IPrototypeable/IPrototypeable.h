#pragma once

// 프로토타입 인터페이스
class IPrototypeable
{
public:
    virtual ~IPrototypeable() = default;
    virtual unique_ptr<IPrototypeable> Clone() const = 0;
    virtual void CopyFrom(const IPrototypeable* source) = 0;
};