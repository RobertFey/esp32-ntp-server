#pragma once

#include <Arduino.h>

class NtpPacket
{
public:
    static const uint16_t SIZE = 48;

    NtpPacket();

    uint8_t* data();

    void clear();
    void load(const uint8_t* buffer);
    void setClientRequest();
    void setServerResponse(uint32_t unixTime, const uint8_t* originateTimestamp);
    uint32_t getTransmitUnixTime();

private:
    uint8_t _buffer[SIZE];

    static const uint32_t NTP_EPOCH_OFFSET = 2208988800UL;
    void writeUint32(int offset, uint32_t value);
    uint32_t readUint32(int offset);
    void writeTimestamp(int offset, uint32_t unixTime);
};