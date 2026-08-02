#include "WifiCliServer.h"
#include "WifiCommandInterface.h"

#include "../cli/Cli.h"

extern Cli cli;

WifiCliServer::WifiCliServer(uint16_t port)
    : _server(port)
{
}

void WifiCliServer::begin()
{
    _server.begin();

    _rxBuffer.reserve(MAX_COMMAND_LENGTH);

    Serial.println("WiFi CLI server started");
}

void WifiCliServer::process()
{
    if (!_client || !_client.connected())
    {
        acceptClient();
        return;
    }

    handleClientInput();
}

void WifiCliServer::acceptClient()
{
    WiFiClient newClient = _server.available();

    if (newClient)
    {
        _client = newClient;
        _rxBuffer.clear();

        _client.println("");
        _client.println("ESP32 NTP Server CLI over WiFi");
        _client.println("Type 'help' for commands");
        _client.println("");
        _client.print("> ");
    }
}

void WifiCliServer::handleClientInput()
{
    WifiCommandInterface wifiIo(_client);

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

            if (_rxBuffer.length() > 0)
            {
                if (_rxBuffer == "exit")
                {
                    disconnectClient("Bye");
                    return;
                }

                cli.executeCommand(_rxBuffer, wifiIo);
            }

            _rxBuffer.clear();

            if (_client && _client.connected())
            {
                _client.print("> ");
            }

            return;
        }

        if (c < 32 || c > 126)
        {
            wifiIo.println("Invalid input character");
            _rxBuffer.clear();
            _client.print("> ");
            return;
        }

        if (_rxBuffer.length() >= MAX_COMMAND_LENGTH)
        {
            wifiIo.println("Error: command too long");
            _rxBuffer.clear();
            _client.print("> ");
            return;
        }

        _rxBuffer += c;
    }
}

void WifiCliServer::disconnectClient(const String& reason)
{
    if (_client && _client.connected())
    {
        _client.println(reason);
        delay(100);
        _client.stop();
    }

    _rxBuffer.clear();
}