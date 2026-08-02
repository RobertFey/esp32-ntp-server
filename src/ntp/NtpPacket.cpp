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

void NtpPacket::writeTimestamp(int offset, uint32_t unixTime, uint16_t millisPart)
{
    uint32_t ntpSeconds = unixTime + NTP_EPOCH_OFFSET;
    writeUint32(offset, ntpSeconds);
    uint32_t ntpFraction = ((uint64_t)millisPart << 32) / 1000;
    writeUint32(offset + 4, ntpFraction);
}

uint32_t NtpPacket::getTransmitUnixTime()
{
    uint32_t ntpSeconds = readUint32(40);
    return ntpSeconds - NTP_EPOCH_OFFSET;
}

void NtpPacket::setServerResponse(const NtpTimestamp& referenceTimestamp, const NtpTimestamp& receiveTimestamp, const NtpTimestamp& transmitTimestamp, const uint8_t* originateTimestamp)
{
    clear();

    _buffer[0] = 0x1C;  // LI=0 VN=3 Mode=4 (server)
    _buffer[1] = 2;     // Stratum 2
    _buffer[2] = 6;     // Poll interval
    _buffer[3] = 0xEC;  // Precision

    writeUint32(4, 0x00010000); // Root Delay
    writeUint32(8, 0x00010000); // Root Dispersion

    _buffer[12] = 'D'; // Reference ID
    _buffer[13] = 'S'; // Reference ID
    _buffer[14] = '3'; // Reference ID 
    _buffer[15] = '2'; // Reference ID

    writeTimestamp(16, referenceTimestamp.unixTime, referenceTimestamp.millisPart); // Reference Timestamp
    memcpy(&_buffer[24], originateTimestamp, 8);                                    // Originate Timestamp
    writeTimestamp(32, receiveTimestamp.unixTime, receiveTimestamp.millisPart);     // Receive Timestamp
    writeTimestamp(40, transmitTimestamp.unixTime, transmitTimestamp.millisPart);   // Transmit Timestamp
}

void NtpPacket::setClientRequest()
{
    clear();

    // LI=0 VN=3 Mode=3 (client)
    _buffer[0] = 0x1B;
}

uint16_t NtpPacket::getTransmitMilliseconds()
{
    uint32_t fraction = readUint32(44);
    return ((uint64_t)fraction * 1000ULL) >> 32;
}