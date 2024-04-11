#include "Detector.h"
#include "RakkPirahPlusKeyboardController.h"
#include "RGBController.h"
#include "RGBController_RakkPirahPlusKeyboard.h"

#define RAKK_PIRAH_PLUS_VID_1 0x258A
#define RAKK_PIRAH_PLUS_PID_1 0x002A
#define RAKK_PIRAH_PLUS_PID_2 0x5088

void DetectRakkPirahPlusKeyboardControllers(hid_device_info* info, const std::string& name)
{
    hid_device* dev = hid_open_path(info->path);

    if(dev)
    {
        RakkPirahPlusKeyboardController*     controller      = new RakkPirahPlusKeyboardController(dev, *info);
        RGBController_RakkPirahPlusKeyboard* rgb_controller  = new RGBController_RakkPirahPlusKeyboard(controller);
        rgb_controller->name                            = name;
        ResourceManager::get()->RegisterRGBController(rgb_controller);
    }
}

REGISTER_HID_DETECTOR_IPU("RakkPirah+ USB", DetectRakkPirahPlusKeyboardControllers, RAKK_PIRAH_PLUS_VID_1, RAKK_PIRAH_PLUS_PID_1, 0, 0x0001, 0x06);
REGISTER_HID_DETECTOR_IPU("RakkPirah+ 2.4g", DetectRakkPirahPlusKeyboardControllers, RAKK_PIRAH_PLUS_VID_1, RAKK_PIRAH_PLUS_PID_2, 0, 0x0001, 0x06);
