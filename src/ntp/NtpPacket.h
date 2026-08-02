#pragma once

#include <Arduino.h>

struct NtpTimestamp
{
    uint32_t unixTime;
    uint16_t millisPart;
};

class NtpPacket
{
public:
    static const uint16_t SIZE = 48;

    NtpPacket();

    uint8_t* data();

    void clear();
    void load(const uint8_t* buffer);
    void setClientRequest();
    // void setServerResponse(uint32_t unixTime, const uint8_t* originateTimestamp);
    void setServerResponse(const NtpTimestamp& referenceTimestamp, const NtpTimestamp& receiveTimestamp, const NtpTimestamp& transmitTimestamp, const uint8_t* originateTimestamp);
    uint32_t getTransmitUnixTime();
    uint16_t getTransmitMilliseconds();

private:
    uint8_t _buffer[SIZE];

    static const uint32_t NTP_EPOCH_OFFSET = 2208988800UL;
    void writeUint32(int offset, uint32_t value);
    uint32_t readUint32(int offset);
    void writeTimestamp(int offset, uint32_t unixTime, uint16_t millisPart);
};