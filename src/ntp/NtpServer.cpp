#include <Arduino.h>
#include "NtpServer.h"
#include "../rtc/RTCManager.h"

extern RTCManager rtcManager;


// Verschil tussen Unix epoch 1970 en NTP epoch 1900
static const uint32_t NTP_EPOCH_OFFSET = 2208988800UL;

bool NtpServer::begin()
{
    return udp.begin(NTP_PORT);
}

void NtpServer::process()
{
    int packetSize = udp.parsePacket();

    if (packetSize <= 0)
    {
        return;
    }

    Serial.print("UDP packet received, size=");
    Serial.println(packetSize);

    if (packetSize < NTP_PACKET_SIZE)
    {
        while (udp.available())
        {
            udp.read();
        }

        Serial.println("NTP packet too small");
        return;
    }

    udp.read(packetBuffer, NTP_PACKET_SIZE);

    IPAddress remoteIp = udp.remoteIP();
    uint16_t remotePort = udp.remotePort();

    Serial.println("NTP request received");

    Serial.print("Byte0: 0x");
    Serial.println(packetBuffer[0], HEX);

    Serial.print("Remote: ");
    Serial.print(remoteIp);
    Serial.print(":");
    Serial.println(remotePort);

    sendResponse(remoteIp, remotePort);

    while (udp.available())
    {
        udp.read();
    }
}

void NtpServer::sendResponse(
    IPAddress remoteIp,
    uint16_t remotePort)
{
    DateTime now = rtcManager.now();
    uint32_t unixTime = now.unixtime();

    uint8_t response[NTP_PACKET_SIZE];
    memset(response, 0, NTP_PACKET_SIZE);

    // Voor test: NTP version 3 server response
    response[0] = 0x1C; // LI=0, VN=3, Mode=4

    response[1] = 2;    // Stratum
    response[2] = 6;    // Poll
    response[3] = 0xEC; // Precision

    writeUint32(response, 4, 0x00010000);
    writeUint32(response, 8, 0x00010000);

    response[12] = 'D';
    response[13] = 'S';
    response[14] = '3';
    response[15] = '2';

    writeNtpTimestamp(response, 16, unixTime);

    // Originate timestamp = client transmit timestamp
    for (int i = 0; i < 8; i++)
    {
        response[24 + i] = packetBuffer[40 + i];
    }

    writeNtpTimestamp(response, 32, unixTime);
    writeNtpTimestamp(response, 40, unixTime);

    Serial.print("Response Byte0: 0x");
    Serial.println(response[0], HEX);

    Serial.print("Unix time: ");
    Serial.println(unixTime);

    Serial.print("Sending to: ");
    Serial.print(remoteIp);
    Serial.print(":");
    Serial.println(remotePort);

    int beginResult = udp.beginPacket(remoteIp, remotePort);

    Serial.print("beginPacket result: ");
    Serial.println(beginResult);

    size_t written = udp.write(response, NTP_PACKET_SIZE);

    Serial.print("UDP bytes written: ");
    Serial.println(written);

    int endResult = udp.endPacket();

    Serial.print("endPacket result: ");
    Serial.println(endResult);

    if (endResult == 0)
    {
        Serial.println("UDP send failed, restarting UDP socket");

        udp.stop();
        delay(100);
        udp.begin(NTP_PORT);
    }
}
void NtpServer::writeUint32(
    uint8_t* buffer,
    int offset,
    uint32_t value)
{
    buffer[offset + 0] = (value >> 24) & 0xFF;
    buffer[offset + 1] = (value >> 16) & 0xFF;
    buffer[offset + 2] = (value >> 8) & 0xFF;
    buffer[offset + 3] = value & 0xFF;
}

void NtpServer::writeNtpTimestamp(
    uint8_t* buffer,
    int offset,
    uint32_t unixTime)
{
    uint32_t ntpSeconds = unixTime + NTP_EPOCH_OFFSET;

    writeUint32(buffer, offset, ntpSeconds);

    // Fractionele seconden.
    // Voor deze eerste versie gebruiken we 0.
    writeUint32(buffer, offset + 4, 0);
}