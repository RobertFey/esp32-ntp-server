#pragma once

#include <Arduino.h>
#include <EthernetUdp.h>
#include <WiFiUdp.h>
#include <IPAddress.h>
#include "NtpPacket.h"

class NtpServer
{
    public:
        bool begin();
        void process();

        uint32_t requestCount();
        uint32_t responseCount();
        uint32_t sendErrorCount();
        String lastClient();
        
        void setDebug(bool value);

    private:
        static const uint16_t NTP_PORT = 123;
        static const uint16_t NTP_PACKET_SIZE = 48;
        
        EthernetUDP ethernetUdp;
        WiFiUDP wifiUdp;
        NtpPacket ntpPacket;
        uint8_t packetBuffer[NTP_PACKET_SIZE];
        
        bool _debug = false;
        bool ethernetStarted = false;
        bool wifiStarted = false;
        
        uint32_t _sendErrorCount = 0;
        uint32_t _requestCount = 0;
        uint32_t _responseCount = 0;
        String _lastClient = "";

        void processEthernet();
        void processWifi();

        void sendEthernetResponse(IPAddress remoteIp, uint16_t remotePort, const NtpTimestamp& receiveTimestamp);
        void sendWifiResponse(IPAddress remoteIp, uint16_t remotePort, const NtpTimestamp& receiveTimestamp);
        void buildResponse( uint8_t* response, uint32_t unixTime);
        void writeUint32( uint8_t* buffer, int offset, uint32_t value);
        void writeNtpTimestamp( uint8_t* buffer, int offset, uint32_t unixTime);
};