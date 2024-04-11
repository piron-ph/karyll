/*-----------------------------------------*\
|  RakkPirahPlusKeyboardController.cpp      |
|                                           |
| Ligolas Neo Ivanovich Mordashov Malicdem  |
| 3/13/2024                                 |
\*-----------------------------------------*/

/*REVERSED BY LIGOLAS NEO MALICDEM*/
#include "RakkPirahPlusKeyboardController.h"
#include <string.h>

#define NA      0xFFFFFFFF

using namespace std::chrono_literals;

RakkPirahPlusKeyboardController::RakkPirahPlusKeyboardController(hid_device* dev_handle, const hid_device_info& info)
{
    dev                 = dev_handle;
    version             = "";
    location            = info.path;

    wchar_t serial_string[128];
    int ret = hid_get_serial_number_string(dev, serial_string, 128);

    if(ret != 0)
    {
        serial_number = "";
    }
    else
    {
        std::wstring return_wstring = serial_string;
        serial_number = std::string(return_wstring.begin(), return_wstring.end());
    }
}

RakkPirahPlusKeyboardController::~RakkPirahPlusKeyboardController()
{
    hid_close(dev);
}

std::string RakkPirahPlusKeyboardController::GetDeviceLocation()
{
    return("HID: " + location);
}

std::string RakkPirahPlusKeyboardController::GetSerialString()
{
    return(serial_number);
}

std::string RakkPirahPlusKeyboardController::GetFirmwareVersion()
{
    return(version);
}

void RakkPirahPlusKeyboardController::StartEffectCommand()
{
    unsigned char usb_buf[PACKET_DATA_LENGTH];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    usb_buf[0x00] = PACKET_HEADER;
    usb_buf[0x01] = LED_EFFECT_START_COMMAND;

    Send(usb_buf);
}

void RakkPirahPlusKeyboardController::StartEffectPage()
{
    /*-----------------------------------------*\
    |  LED_SPECIAL_EFFECT_PACKETS:              |
    |   Packet amount that will be sent in this |
    |   transaction                             |
    \*-----------------------------------------*/
    unsigned char usb_buf[PACKET_DATA_LENGTH];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    usb_buf[0x00] = PACKET_HEADER;
    usb_buf[0x01] = WRITE_LED_SPECIAL_EFFECT_AREA_COMMAND;
    usb_buf[0x08] = LED_SPECIAL_EFFECT_PACKETS;

    Send(usb_buf);

    Read();
}

void RakkPirahPlusKeyboardController::SetCustomization(bool state)
{
    unsigned char usb_buf[PACKET_DATA_LENGTH];

    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);
    usb_buf[0x00] = PACKET_HEADER;
    usb_buf[0x01] = state ? TURN_ON_CUSTOMIZATION_COMMAND : TURN_OFF_CUSTOMIZATION_COMMAND;

    Send(usb_buf);

    Read();
}

void RakkPirahPlusKeyboardController::EndCommunication()
{
    unsigned char usb_buf[PACKET_DATA_LENGTH];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    usb_buf[0x00] = PACKET_HEADER;
    usb_buf[0x01] = COMMUNICATION_END_COMMAND;

    Send(usb_buf);

    Read();
}

void RakkPirahPlusKeyboardController::Read()
{
    unsigned char usb_buf[PACKET_DATA_LENGTH+1];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH+1);

    usb_buf[0x00] = REPORT_ID;

    hid_get_feature_report(dev, usb_buf, PACKET_DATA_LENGTH+1);
}

void RakkPirahPlusKeyboardController::Send(unsigned char data[PACKET_DATA_LENGTH])
{
    unsigned char usb_buf[PACKET_DATA_LENGTH+1];

    usb_buf[0] = REPORT_ID;

    for(unsigned int x = 0; x < PACKET_DATA_LENGTH; x++)
    {
        usb_buf[x+1] = data[x];
    }

    hid_send_feature_report(dev, usb_buf, PACKET_DATA_LENGTH+1);
}

void RakkPirahPlusKeyboardController::SendLEDsBuffer(std::vector<RGBColor> colors, std::vector<unsigned int> pos)
{
    unsigned char usb_buf[PACKET_DATA_LENGTH];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    unsigned char color_buf[COLOR_BUF_SIZE];
    memset(color_buf, 0x00, COLOR_BUF_SIZE);

    for(unsigned int l = 0; l < pos.size(); l++)
    {
        if(pos[l] != NA)
        {
            color_buf[l * 4]       = l;
            color_buf[l * 4 + 1]   = RGBGetRValue(colors[pos[l]]);
            color_buf[l * 4 + 2]   = RGBGetGValue(colors[pos[l]]);
            color_buf[l * 4 + 3]   = RGBGetBValue(colors[pos[l]]);
        }
    }

    for(unsigned int p = 0; p < 8; p++)
    {
        memcpy(usb_buf, &color_buf[p * PACKET_DATA_LENGTH], PACKET_DATA_LENGTH);
        Send(usb_buf);
    }
}
void RakkPirahPlusKeyboardController::UpdateMode(std::vector<mode> modes, int active_mode)
{
    SetCustomization(TURN_ON_CUSTOMIZATION_COMMAND);
    StartEffectPage();

    unsigned char usb_buf[PACKET_DATA_LENGTH];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    usb_buf[0] = modes[active_mode].value;   // mode value

    if(modes[active_mode].flags & MODE_FLAG_HAS_MODE_SPECIFIC_COLOR)
    {
        usb_buf[1] = RGBGetRValue(modes[active_mode].colors[0]);
        usb_buf[2] = RGBGetGValue(modes[active_mode].colors[0]);
        usb_buf[3] = RGBGetBValue(modes[active_mode].colors[0]);
    }

    usb_buf[8]  = modes[active_mode].color_mode == MODE_COLORS_RANDOM;  // random switch
    usb_buf[9]  = modes[active_mode].brightness;
    usb_buf[10] = modes[active_mode].speed;
    usb_buf[11] = modes[active_mode].direction;

    usb_buf[14] = EFFECT_PAGE_CHECK_CODE_L;
    usb_buf[15] = EFFECT_PAGE_CHECK_CODE_H;
    Send(usb_buf);

    Read();

    EndCommunication();

    StartEffectCommand();
}

void RakkPirahPlusKeyboardController::SendDirect(std::vector<RGBColor> colors, std::vector<unsigned int> pos)
{
    unsigned char usb_buf[PACKET_DATA_LENGTH];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    usb_buf[0] = PACKET_HEADER;
    usb_buf[1] = DIRECT_MODE_VALUE;
    usb_buf[8] = 0x08;

    Send(usb_buf);

    SendLEDsBuffer(colors, pos);

    Read();

    EndCommunication();
}

void RakkPirahPlusKeyboardController::SendCustom(std::vector<RGBColor> colors, std::vector<unsigned int> pos)
{
    SetCustomization(TURN_ON_CUSTOMIZATION_COMMAND);

    unsigned char usb_buf[PACKET_DATA_LENGTH];
    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    usb_buf[0] = PACKET_HEADER;
    usb_buf[1] = CUSTOM_MODE_VALUE;
    usb_buf[8] = 0x09;

    Send(usb_buf);

    SendLEDsBuffer(colors, pos);

    Read();

    EndCommunication();

    StartEffectCommand();

    SetCustomization(TURN_ON_CUSTOMIZATION_COMMAND);

    StartEffectPage();

    memset(usb_buf, 0x00, PACKET_DATA_LENGTH);

    usb_buf[0]  = LIGHTS_OFF_MODE_VALUE;
    usb_buf[1]  = 0xFF;
    usb_buf[8]  = 0x01;
    usb_buf[9]  = 0x0F;
    usb_buf[10] = 0x0F;
    usb_buf[14] = EFFECT_PAGE_CHECK_CODE_L;
    usb_buf[15] = EFFECT_PAGE_CHECK_CODE_H;

    Send(usb_buf);

    Read();

    EndCommunication();

    StartEffectCommand();
}
