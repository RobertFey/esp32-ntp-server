#include "IndicatorManager.h"

void IndicatorManager::begin()
{
    pinMode(RUN_LED_PIN, OUTPUT);
    pinMode(RX_LED_PIN, OUTPUT);
    pinMode(TX_LED_PIN, OUTPUT);
    pinMode(ERROR_LED_PIN, OUTPUT);

    digitalWrite(RUN_LED_PIN, LOW);
    digitalWrite(RX_LED_PIN, LOW);
    digitalWrite(TX_LED_PIN, LOW);
    digitalWrite(ERROR_LED_PIN, LOW);
}

void IndicatorManager::requestReceived()
{
    digitalWrite(RX_LED_PIN, HIGH);

    _rxActive = true;
    _rxUntil = millis() + 50;
}

void IndicatorManager::responseSent()
{
    digitalWrite(TX_LED_PIN, HIGH);

    _txActive = true;
    _txUntil = millis() + 50;
}

void IndicatorManager::error()
{
    digitalWrite(ERROR_LED_PIN, HIGH);

    _errorActive = true;
    _errorUntil = millis() + 250;
}

void IndicatorManager::setRunMode(RunMode mode)
{
    _runMode = mode;
}

void IndicatorManager::process()
{
    unsigned long now = millis();
    unsigned long runInterval;

    switch (_runMode)
    {
        case RunMode::NoNetwork:
            runInterval = 1000;
            break;

        case RunMode::Normal:
            runInterval = 500;
            break;

        case RunMode::Syncing:
            runInterval = 100;
            break;

        default:
            runInterval = 500;
            break;
    }
    
    //
    // RUN LED
    //
    if ((now - _lastRunToggle) >= runInterval)
    {
        _lastRunToggle = now;
        _runState = !_runState;
        digitalWrite(RUN_LED_PIN, _runState);
    }

    //
    // RX LED
    //
    if (_rxActive && now >= _rxUntil)
    {
        digitalWrite(RX_LED_PIN, LOW);
        _rxActive = false;
    }

    //
    // TX LED
    //
    if (_txActive && now >= _txUntil)
    {
        digitalWrite(TX_LED_PIN, LOW);
        _txActive = false;
    }

    //
    // ERROR LED
    //
    if (_errorActive && now >= _errorUntil)
    {
        digitalWrite(ERROR_LED_PIN, LOW);
        _errorActive = false;
    }
}

