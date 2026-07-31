#include "TcpCliServer.h"

#include "../cli/Cli.h"
#include "TcpCommandInterface.h"

extern Cli cli;

void TcpServer::begin()
{
    _server.begin();

    Serial.println("TCP CLI started on port 23");
}

void TcpServer::process()
{
    if (!_client || !_client.connected())
    {
        _client = _server.available();

        if (_client)
        {
            _client.println();
            _client.println("ESP32 NTP Server");
            _client.println("Type 'help'");
            _client.print("> ");

            _rxBuffer = "";
        }
    }

    // Client disconnected?
    if (_client && !_client.connected())
    {
        Serial.println("TCP client disconnected");

        _client.stop();
        _rxBuffer = "";
    }

    // Nieuwe client accepteren
    if (!_client)
    {
        return;
    }

    while (_client.available())
    {
        char c = _client.read();

        if (c == '\r')
        {
            continue;
        }

        if (c == '\n')
        {
            _rxBuffer.trim();

            if (!_rxBuffer.isEmpty())
            {
                TcpCommandInterface io(_client);

                cli.executeCommand(_rxBuffer, io);
            }

            _client.print("> ");
            _rxBuffer = "";
        }
        else
        {
            _rxBuffer += c;
        }
    }
}
