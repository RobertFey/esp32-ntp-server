#include "RTCManager.h"

bool RTCManager::hasLostPower()
{
    return rtc.lostPower();
}

float RTCManager::getTemperature()
{
    return rtc.getTemperature();
}

DateTime RTCManager::now()
{
    return rtc.now();
}

bool RTCManager::begin()
{
    return rtc.begin();
}

String RTCManager::getDateTimeString()
{
    DateTime dt = rtc.now();

    char buffer[32];

    snprintf(
        buffer,
        sizeof(buffer),
        "%04u-%02u-%02u %02u:%02u:%02u",
        dt.year(),
        dt.month(),
        dt.day(),
        dt.hour(),
        dt.minute(),
        dt.second());

    return String(buffer);
}

bool RTCManager::setTime(
    uint16_t year,
    uint8_t month,
    uint8_t day,
    uint8_t hour,
    uint8_t minute,
    uint8_t second)
{
    rtc.adjust(
        DateTime(
            year,
            month,
            day,
            hour,
            minute,
            second));

    return true;
}