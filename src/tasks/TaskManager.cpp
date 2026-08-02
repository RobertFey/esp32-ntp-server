#include "TaskManager.h"

#include "../ntp/NtpServer.h"
#include "../ntp/NtpClient.h"
#include "../tcp/TcpCliServer.h"
#include "../indicators/IndicatorManager.h"

extern NtpServer ntpServer;
extern NtpClient ntpClient;
extern TcpCliServer tcpCliServer;
extern IndicatorManager indicatorManager;

static TaskHandle_t ntpTaskHandle = nullptr;
static TaskHandle_t cliTaskHandle = nullptr;
static TaskHandle_t statusTaskHandle = nullptr;


static void ntpTask(void* parameter)
{
    while (true)
    {
        ntpServer.process();
        vTaskDelay(pdMS_TO_TICKS(1));
    }
}

static void cliTask(void* parameter)
{
    while (true)
    {
        tcpCliServer.process();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void statusTask(void* parameter)
{
    while (true)
    {
        ntpClient.process();
        indicatorManager.process();
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void TaskManager::begin()
{
    xTaskCreatePinnedToCore(ntpTask, "NTP", 4096, nullptr, 3, &ntpTaskHandle, 1);
    xTaskCreatePinnedToCore(cliTask, "CLI", 4096, nullptr, 2, &cliTaskHandle, 1);
    xTaskCreatePinnedToCore(statusTask, "STATUS", 4096, nullptr, 1, &statusTaskHandle, 1);
}