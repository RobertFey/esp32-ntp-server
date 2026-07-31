#pragma once

#include <Arduino.h>

class ICommandInterface
{
public:
    virtual ~ICommandInterface() = default;

    virtual void print(const String& text) = 0;
    virtual void println(const String& text) = 0;
};