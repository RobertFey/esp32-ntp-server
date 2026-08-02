#pragma once

#include <Arduino.h>
#include <WiFiUdp.h>


enum class NtpSyncResult
{
    Success,
    NetworkUnavailable,
    DnsLookupFailed,
    SendFailed,
    Timeout,
    InvalidResponse,
    RtcUpdateFailed
};

class NtpClient
{
public:
    bool syncRtc();                 // oude API
    NtpSyncResult syncRtcEx();      // nieuwe API

    String resultToString(NtpSyncResult result);
    String lastSyncTime();
    bool lastSyncSuccess();
    NtpSyncResult lastResult();
    
    void process();

private:
    unsigned long _lastSyncMillis = 0;
    bool _firstRun = true;
    NtpSyncResult _lastResult = NtpSyncResult::Success;
    static const uint16_t NTP_PORT = 123;
    static const uint16_t NTP_PACKET_SIZE = 48;

    WiFiUDP udp;

    String _lastSyncTime = "";
    bool _lastSyncSuccess = false;
    bool queryServer(const String& hostname, uint32_t& unixTime);
    void restoreRunMode();
};

