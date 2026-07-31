#include "TcpCommandInterface.h"

TcpCommandInterface::TcpCommandInterface(
    EthernetClient& client)
    : _client(client)
{
}

void TcpCommandInterface::print(
    const String& text)
{
    _client.print(text);
}

void TcpCommandInterface::println(
    const String& text)
{
    _client.println(text);
}