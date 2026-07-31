#pragma once

#include <Ethernet.h>
#include "../cli/ICommandInterface.h"

class TcpCommandInterface : public ICommandInterface
{
public:
    explicit TcpCommandInterface(EthernetClient& client);

    void print(const String& text) override;
    void println(const String& text) override;

private:
    EthernetClient& _client;
};