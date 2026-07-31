#include "SerialCommandInterface.h"

void SerialCommandInterface::print(const String& text)
{
    Serial.print(text);
}

void SerialCommandInterface::println(const String& text)
{
    Serial.println(text);
}