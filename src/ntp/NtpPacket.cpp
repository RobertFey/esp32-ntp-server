#include "NtpPacket.h"

static const uint32_t NTP_EPOCH_OFFSET = 2208988800UL;

NtpPacket::NtpPacket()
{
    clear();
}

uint8_t* NtpPacket::data()
{
    return _buffer;
}

void NtpPacket::clear()
{
    memset(_buffer, 0, SIZE);
}

void NtpPacket::load(const uint8_t* buffer)
{
    memcpy(_buffer, buffer, SIZE);
}

uint32_t NtpPacket::readUint32(int offset)
{
    return
        ((uint32_t)_buffer[offset] << 24) |
        ((uint32_t)_buffer[offset + 1] << 16) |
        ((uint32_t)_buffer[offset + 2] << 8) |
        ((uint32_t)_buffer[offset + 3]);
}

void NtpPacket::writeUint32(int offset, uint32_t value)
{
    _buffer[offset]     = (value >> 24) & 0xFF;
    _buffer[offset + 1] = (value >> 16) & 0xFF;
    _buffer[offset + 2] = (value >> 8) & 0xFF;
    _buffer[offset + 3] = value & 0xFF;
}

void NtpPacket::writeTimestamp(int offset, uint32_t unixTime)
{
    uint32_t ntpSeconds = unixTime + NTP_EPOCH_OFFSET;
    writeUint32(offset,ntpSeconds);
    // fractionele seconden
    writeUint32(offset + 4, 0);
}

uint32_t NtpPacket::getTransmitUnixTime()
{
    uint32_t ntpSeconds = readUint32(40);
    return ntpSeconds - NTP_EPOCH_OFFSET;
}

void NtpPacket::setServerResponse(uint32_t unixTime, const uint8_t* originateTimestamp)
{
    clear();

    _buffer[0] = 0x1C;
    _buffer[1] = 2;
    _buffer[2] = 6;
    _buffer[3] = 0xEC;

    writeUint32(4, 0x00010000);
    writeUint32(8, 0x00010000);

    _buffer[12] = 'D';
    _buffer[13] = 'S';
    _buffer[14] = '3';
    _buffer[15] = '2';

    writeTimestamp(16, unixTime);

    memcpy(&_buffer[24], originateTimestamp, 8);

    writeTimestamp(32, unixTime);
    writeTimestamp(40, unixTime);
}

void NtpPacket::setClientRequest()
{
    clear();

    // LI=0 VN=3 Mode=3 (client)
    _buffer[0] = 0x1B;
}