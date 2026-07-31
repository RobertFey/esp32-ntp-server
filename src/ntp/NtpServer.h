#pragma once

#include <Arduino.h>
#include <EthernetUdp.h>

class NtpServer
{
public:
    bool begin();
    void process();

private:
    static const uint16_t NTP_PORT = 123;
    static const uint16_t NTP_PACKET_SIZE = 48;

    EthernetUDP udp;
    uint8_t packetBuffer[NTP_PACKET_SIZE];

    void sendResponse(IPAddress remoteIp, uint16_t remotePort);

    void writeUint32(
        uint8_t* buffer,
        int offset,
        uint32_t value);

    void writeNtpTimestamp(
        uint8_t* buffer,
        int offset,
        uint32_t unixTime);
};