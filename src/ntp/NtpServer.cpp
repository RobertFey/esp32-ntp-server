#include "NtpServer.h"
#include "NtpPacket.h"
#include "../rtc/RTCManager.h"
#include "../network/NetworkManager.h"

extern RTCManager rtcManager;
extern NetworkManager networkManager;

static const uint32_t NTP_EPOCH_OFFSET = 2208988800UL;

void NtpServer::writeUint32( uint8_t* buffer, int offset, uint32_t value)
{
    // Big-endian schrijven
    // Byte 0: LI, VN, Mode=4 (server)  
    buffer[offset + 0] = (value >> 24) & 0xFF;
    // Byte 1: Stratum=1 (Primary reference - RTC)
    buffer[offset + 1] = (value >> 16) & 0xFF;
    // Byte 2: Poll interval (6 = 64s)
    buffer[offset + 2] = (value >> 8) & 0xFF;
    // Byte 3: Precision (0xEC = -20, ~1us)
    buffer[offset + 3] = value & 0xFF;
}

void NtpServer::writeNtpTimestamp( uint8_t* buffer, int offset, uint32_t unixTime)
{
    uint32_t ntpSeconds = unixTime + NTP_EPOCH_OFFSET;

    writeUint32(buffer, offset, ntpSeconds);

    // Fractionele seconden voorlopig 0
    writeUint32(buffer, offset + 4, 0);
}

bool NtpServer::begin()
{
    bool started = false;

    if (networkManager.ethernetConnected())
    {
        ethernetStarted = ethernetUdp.begin(NTP_PORT);

        if (ethernetStarted)
        {
            Serial.println("NTP Ethernet listener started");
            started = true;
        }
        else
        {
            Serial.println("NTP Ethernet listener failed");
        }
    }

    if (networkManager.wifiConnected())
    {
        wifiStarted = wifiUdp.begin(NTP_PORT);

        if (wifiStarted)
        {
            Serial.println("NTP WiFi listener started");
            started = true;
        }
        else
        {
            Serial.println("NTP WiFi listener failed");
        }
    }

    return started;
}

void NtpServer::process()
{
    if (ethernetStarted && networkManager.ethernetConnected())
    {
        processEthernet();
    }

    if (wifiStarted && networkManager.wifiConnected())
    {
        processWifi();
    }
}

void NtpServer::processEthernet()
{
    int packetSize = ethernetUdp.parsePacket();

    if (packetSize <= 0)
    {
        return;
    }

    Serial.print("Ethernet NTP packet, size=");
    Serial.println(packetSize);

    if (packetSize < NTP_PACKET_SIZE)
    {
        while (ethernetUdp.available())
        {
            ethernetUdp.read();
        }

        Serial.println("Ethernet NTP packet too small");
        return;
    }

    ethernetUdp.read(packetBuffer, NTP_PACKET_SIZE);

    IPAddress remoteIp = ethernetUdp.remoteIP();
    uint16_t remotePort = ethernetUdp.remotePort();

    _requestCount++;
    _lastClient = remoteIp.toString();

    Serial.print("Ethernet NTP request from ");
    Serial.print(remoteIp);
    Serial.print(":");
    Serial.println(remotePort);

    sendEthernetResponse(remoteIp, remotePort);

    while (ethernetUdp.available())
    {
        ethernetUdp.read();
    }
}

void NtpServer::processWifi()
{
    int packetSize = wifiUdp.parsePacket();

    if (packetSize <= 0)
    {
        return;
    }
    
    if (packetSize < NTP_PACKET_SIZE)
    {
        while (wifiUdp.available())
        {
            wifiUdp.read();
        }
        
        Serial.println("WiFi NTP packet too small");
        return;
    }
    
    wifiUdp.read(packetBuffer, NTP_PACKET_SIZE);
    
    IPAddress remoteIp = wifiUdp.remoteIP();
    uint16_t remotePort = wifiUdp.remotePort();
    
    _requestCount++;
    _lastClient = remoteIp.toString();
    
    if (_debug)
    {
        Serial.print("WiFi NTP packet, size=");
        Serial.println(packetSize);
        Serial.print("WiFi NTP request from ");
        Serial.print(remoteIp);
        Serial.print(":");
        Serial.println(remotePort);
    }

    sendWifiResponse(remoteIp, remotePort);

    while (wifiUdp.available())
    {
        wifiUdp.read();
    }
}

void NtpServer::sendEthernetResponse(IPAddress remoteIp, uint16_t remotePort)
{
    DateTime now = rtcManager.now();
    NtpPacket packet;

    packet.setServerResponse(now.unixtime(), &packetBuffer[40]);
    ethernetUdp.beginPacket(remoteIp, remotePort);
    ethernetUdp.write(packet.data(), NtpPacket::SIZE);
    int endResult = ethernetUdp.endPacket();
    if (endResult == 0)
    {
        Serial.println("Ethernet UDP send failed, restarting socket");
        ethernetUdp.stop();
        delay(100);
        ethernetStarted = ethernetUdp.begin(NTP_PORT);
        _sendErrorCount++;
    } else
    {
        _responseCount++;
    }
}

void NtpServer::sendWifiResponse(IPAddress remoteIp, uint16_t remotePort)
{
    DateTime now = rtcManager.now();

    NtpPacket packet;

    packet.setServerResponse(now.unixtime(), &packetBuffer[40]);
    wifiUdp.beginPacket(remoteIp, remotePort);
    wifiUdp.write( packet.data(), NtpPacket::SIZE);

    int endResult = wifiUdp.endPacket();
    if (endResult == 0)
    {
        Serial.println("WiFi UDP send failed, restarting socket");
        wifiUdp.stop();
        delay(100);
        wifiStarted = wifiUdp.begin(NTP_PORT);
        _sendErrorCount++;
    } else
    {
        _responseCount++;
    }
}

void NtpServer::buildResponse(uint8_t* response, uint32_t unixTime)
{
    memset(response, 0, NTP_PACKET_SIZE);

    /*
        LI = 0
        VN = 3
        Mode = 4 server

        00 011 100 = 0x1C
    */
    response[0] = 0x1C;

    // Stratum 2 voor eerste stabiele test
    response[1] = 2;

    // Poll interval
    response[2] = 6;

    // Precision
    response[3] = 0xEC;

    // Root Delay
    writeUint32(response, 4, 0x00010000);

    // Root Dispersion
    writeUint32(response, 8, 0x00010000);

    // Reference ID
    response[12] = 'D';
    response[13] = 'S';
    response[14] = '3';
    response[15] = '2';

    // Reference Timestamp
    writeNtpTimestamp(response, 16, unixTime);

    // Originate Timestamp = client transmit timestamp uit request
    for (int i = 0; i < 8; i++)
    {
        response[24 + i] = packetBuffer[40 + i];
    }

    // Receive Timestamp
    writeNtpTimestamp(response, 32, unixTime);

    // Transmit Timestamp
    writeNtpTimestamp(response, 40, unixTime);
}

uint32_t NtpServer::requestCount()
{
    return _requestCount;
}

uint32_t NtpServer::responseCount()
{
    return _responseCount;
}

String NtpServer::lastClient()
{
    return _lastClient;
}

uint32_t NtpServer::sendErrorCount()
{
    return _sendErrorCount;
}

void NtpServer::setDebug(bool value)
{
    _debug = value;
}