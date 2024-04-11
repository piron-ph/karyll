/*-----------------------------------------*\
|  RGBController_RakkPirahPlusKeyboard.h    |
|                                           |
| Ligolas Neo Ivanovich Mordashov Malicdem  |
| 3/13/2024                                 |
\*-----------------------------------------*/
/*REVERSED BY LIGOLAS NEO MALICDEM*/
#pragma once
#include <chrono>

#include "RGBController.h"
#include "RakkPirahPlusKeyboardController.h"

class RGBController_RakkPirahPlusKeyboard : public RGBController
{
public:
    RGBController_RakkPirahPlusKeyboard(RakkPirahPlusKeyboardController* controller_ptr);
    ~RGBController_RakkPirahPlusKeyboard();

    void SetupZones();
    void ResizeZone(int zone, int new_size);

    void DeviceUpdateLEDs();
    void UpdateZoneLEDs(int zone);
    void UpdateSingleLED(int led);
    void DeviceUpdateMode();

    void KeepaliveThread();

private:
    RakkPirahPlusKeyboardController* controller;
    std::thread*                                        keepalive_thread;
    std::atomic<bool>                                   keepalive_thread_run;
    std::chrono::time_point<std::chrono::steady_clock>  last_update_time;
};
