#include "WifiCommandInterface.h"

WifiCommandInterface::WifiCommandInterface(WiFiClient& client)
    : _client(client)
{
}

void WifiCommandInterface::print(const String& text)
{
    if (_client && _client.connected())
    {
        _client.print(text);
    }
}

void WifiCommandInterface::println(const String& text)
{
    if (_client && _client.connected())
    {
        _client.println(text);
    }
}
