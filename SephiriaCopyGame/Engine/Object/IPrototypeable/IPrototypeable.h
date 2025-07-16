#pragma once

class IPrototypeable
{
public:
    virtual ~IPrototypeable() = default;
    virtual unique_ptr<IPrototypeable> Clone() const = 0;
    virtual void CopyFrom(const IPrototypeable* source) = 0;
};