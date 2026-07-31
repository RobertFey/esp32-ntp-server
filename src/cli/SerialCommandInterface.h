#pragma once

#include "ICommandInterface.h"

class SerialCommandInterface : public ICommandInterface
{
public:
    void print(const String& text) override;
    void println(const String& text) override;
};