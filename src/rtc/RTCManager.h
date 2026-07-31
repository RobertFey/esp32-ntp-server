#pragma once

#include <Arduino.h>
#include <RTClib.h>

class RTCManager
{
public:
    bool begin();

    String getDateTimeString();

    bool setTime(
        uint16_t year,
        uint8_t month,
        uint8_t day,
        uint8_t hour,
        uint8_t minute,
        uint8_t second);
    bool hasLostPower();
    float getTemperature();

private:
    RTC_DS3231 rtc;
};
