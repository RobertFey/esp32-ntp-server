#pragma once

#include <Arduino.h>

enum class RunMode
{
    NoNetwork,
    Normal,
    Syncing
};

class IndicatorManager
{
public:
    void begin();
    void process();

    void requestReceived();
    void responseSent();
    void error();
    void setRunMode(RunMode mode);

private:
    RunMode _runMode = RunMode::NoNetwork;
    static constexpr uint8_t RUN_LED_PIN   = 33;
    static constexpr uint8_t RX_LED_PIN    = 25;
    static constexpr uint8_t TX_LED_PIN    = 26;
    static constexpr uint8_t ERROR_LED_PIN = 27;

    bool _runState = false;
    unsigned long _lastRunToggle = 0;

    bool _rxActive = false;
    bool _txActive = false;
    bool _errorActive = false;

    unsigned long _rxUntil = 0;
    unsigned long _txUntil = 0;
    unsigned long _errorUntil = 0;
};