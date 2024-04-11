/*-----------------------------------------*\
|  RGBController_RakkPirahPlusKeyboard.cpp  |
|                                           |
| Ligolas Neo Ivanovich Mordashov Malicdem  |
| 3/13/2024                                 |
\*-----------------------------------------*/
/*REVERSED BY LIGOLAS NEO MALICDEM*/
#include "RGBControllerKeyNames.h"
#include "RGBController_RakkPirahPlusKeyboard.h"

#include <thread>
#include <chrono>

#define NA      0xFFFFFFFF

using namespace std::chrono_literals;

typedef struct
{
    const unsigned int                      width;                         /* matrix width                        */
    const unsigned int                      height;                        /* matrix height                       */
    std::vector<std::vector<unsigned int>>  matrix_map;                    /* matrix map                          */
    std::vector<std::string>                led_names;                     /* led names                           */
    std::vector<unsigned int>               positions_custom;              /* position in buffers for custom mode */

} RakkPirahPlus;

static RakkPirahPlus matrix =
{
    16,
    5,
    {
        {  0,  5,  7, 12, 16, 20, 24, 29, 33, 37, 41, 46, 51, 55, NA, NA},
        {  1, NA,  8, 13, 17, 21, 25, 30, 34, 38, 42, 47, 52, 56, 58, 62},
        {  2, NA,  9, 14, 18, 22, 26, 31, 35, 39, 43, 48, 53, NA, 59, 63},
        {  3, NA, 10, 15, 19, 23, 27, 32, 36, 40, 44, 49, 54, NA, 60, 64},
        {  4,  6, 11, NA, NA, NA, 28, NA, NA, NA, 45, 50, NA, 57, 61, 65}
    },
    {
        KEY_EN_ESCAPE,
        KEY_EN_TAB,
        KEY_EN_CAPS_LOCK,
        KEY_EN_LEFT_SHIFT,
        KEY_EN_LEFT_CONTROL,

        KEY_EN_1,
        KEY_EN_LEFT_WINDOWS,

        KEY_EN_2,
        KEY_EN_Q,
        KEY_EN_A,
        KEY_EN_Z,
        KEY_EN_LEFT_ALT,

        KEY_EN_3,
        KEY_EN_W,
        KEY_EN_S,
        KEY_EN_X,

        KEY_EN_4,
        KEY_EN_E,
        KEY_EN_D,
        KEY_EN_C,

        KEY_EN_5,
        KEY_EN_R,
        KEY_EN_F,
        KEY_EN_V,

        KEY_EN_6,
        KEY_EN_T,
        KEY_EN_G,
        KEY_EN_B,
        KEY_EN_SPACE,

        KEY_EN_7,
        KEY_EN_Y,
        KEY_EN_H,
        KEY_EN_N,

        KEY_EN_8,
        KEY_EN_U,
        KEY_EN_J,
        KEY_EN_M,

        KEY_EN_9,
        KEY_EN_I,
        KEY_EN_K,
        KEY_EN_COMMA,

        KEY_EN_0,
        KEY_EN_O,
        KEY_EN_L,
        KEY_EN_PERIOD,
        KEY_EN_RIGHT_ALT,

        KEY_EN_MINUS,
        KEY_EN_P,
        KEY_EN_SEMICOLON,
        KEY_EN_FORWARD_SLASH,
        KEY_EN_RIGHT_FUNCTION,

        KEY_EN_NUMPAD_PLUS,
        KEY_EN_LEFT_BRACKET,
        KEY_EN_QUOTE,
        KEY_EN_LEFT_SHIFT,

        KEY_EN_BACKSPACE,
        KEY_EN_RIGHT_BRACKET,
        KEY_EN_LEFT_ARROW,

        KEY_EN_ANSI_BACK_SLASH,
        KEY_EN_ISO_ENTER,
        KEY_EN_UP_ARROW,
        KEY_EN_DOWN_ARROW,

        KEY_EN_DELETE,
        KEY_EN_PAGE_UP,
        KEY_EN_PAGE_DOWN,
        KEY_EN_RIGHT_ARROW
    },
    {
        NA,  0, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA, NA,
        NA, NA, NA, NA,  5,  7, 12, 16, 20, 24, 29, 33, 37, 41, 46, 51,
        NA, NA, NA, NA, NA,  1,  8, 13, 17, 21, 25, 30, 34, 38, 42, 47,
        52, 56, NA, NA, NA, NA, NA,  2,  9, 14, 18, 22, 26, 31, 35, 39,
        43, 48, 53, 58, NA, NA, NA, NA, NA,  3, 10, 15, 19, 23, 27, 32,
        36, 40, 44, 49, 54, 59, NA, NA, NA, NA, NA,  4,  6, 11, 28, 45,
        50, NA, NA, 57, 61, 60, 65, 55, NA, NA, NA, NA, NA, NA, NA, NA,
        NA, NA, NA, NA, NA, NA, 63, 62, NA, 64, NA, NA, NA, NA, NA, NA
    }
};

typedef struct
{
    std::string name;
    int value;
    int flags;
} RakkPirahPlus_effect;

/**------------------------------------------------------------------*\
    @name RakkPirahPlus
    @category Keyboard
    @type USB
    @save :robot:
    @direct :white_check_mark:
    @effects :white_check_mark:
    @detectors DetectRakkPirahPlusKeyboardControllers
    @comment
\*-------------------------------------------------------------------*/

RGBController_RakkPirahPlusKeyboard::RGBController_RakkPirahPlusKeyboard(RakkPirahPlusKeyboardController* controller_ptr)
{
    controller                          = controller_ptr;
    name                                = "Rakk Pirah +";
    vendor                              = "RAKK";
    type                                = DEVICE_TYPE_KEYBOARD;
    description                         = "Rakk Pinas!";
    location                            = controller->GetDeviceLocation();
    serial                              = controller->GetSerialString();
    version                             = controller->GetFirmwareVersion();

    mode Direct;
    Direct.name                         = "Direct";
    Direct.value                        = DIRECT_MODE_VALUE;
    Direct.flags                        = MODE_FLAG_HAS_PER_LED_COLOR;
    Direct.color_mode                   = MODE_COLORS_PER_LED;
    modes.push_back(Direct);

    mode Custom;
    Custom.name                         = "Custom";
    Custom.value                        = CUSTOM_MODE_VALUE;
    Custom.flags                        = MODE_FLAG_HAS_PER_LED_COLOR | MODE_FLAG_AUTOMATIC_SAVE;
    Custom.color_mode                   = MODE_COLORS_PER_LED;
    modes.push_back(Custom);

    RakkPirahPlus_effect RakkPirahPlus_effects[20] =
    {
        {
            "Static",
            STATIC_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Keystroke light up",
            KEYSTROKE_LIGHT_UP_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Keystroke dim",
            KEYSTROKE_DIM_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Sparkle",
            SPARKLE_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Rain",
            RAIN_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Random colors",
            RANDOM_COLORS_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Breathing",
            BREATHING_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Spectrum cycle",
            SPECTRUM_CYCLE_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Ring gradient",
            RING_GRADIENT_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Vertical gradient",
            VERTICAL_GRADIENT_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_UD | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Horizontal gradient / Rainbow wave",
            HORIZONTAL_GRADIENT_WAVE_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Around edges",
            AROUND_EDGES_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Keystroke horizontal lines",
            KEYSTROKE_HORIZONTAL_LINES_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Keystroke tilted lines",
            KEYSTROKE_TITLED_LINES_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Keystroke ripples",
            KEYSTROKE_RIPPLES_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Sequence",
            SEQUENCE_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Wave line",
            WAVE_LINE_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Tilted lines",
            TILTED_LINES_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Back and forth",
            BACK_AND_FORTH_MODE_VALUE,
            MODE_FLAG_HAS_RANDOM_COLOR | MODE_FLAG_HAS_MODE_SPECIFIC_COLOR | MODE_FLAG_HAS_BRIGHTNESS | MODE_FLAG_HAS_SPEED | MODE_FLAG_HAS_DIRECTION_LR | MODE_FLAG_AUTOMATIC_SAVE
        },
        {
            "Off",
            LIGHTS_OFF_MODE_VALUE,
            MODE_FLAG_AUTOMATIC_SAVE
        }
    };

    for(const RakkPirahPlus_effect& effect : RakkPirahPlus_effects)
    {
        mode m;
        m.name = effect.name;
        m.value = effect.value;
        m.flags = effect.flags;

        if(m.flags & MODE_FLAG_HAS_MODE_SPECIFIC_COLOR)
        {
            m.color_mode = MODE_COLORS_MODE_SPECIFIC;
            m.colors_min = 1;
            m.colors_max = 1;
            m.colors.resize(1);
        }
        else
        {
            m.color_mode = MODE_COLORS_NONE;
            m.colors_min = 0;
            m.colors_max = 0;
            m.colors.resize(0);
        }

        if(m.flags & MODE_FLAG_HAS_SPEED)
        {
            m.speed_min = RakkPirahPlus_MIN_SPEED;
            m.speed_max = RakkPirahPlus_MAX_SPEED;
            m.speed = m.speed_min;
        }

        if(m.flags & MODE_FLAG_HAS_BRIGHTNESS)
        {
            m.brightness_min = RakkPirahPlus_MIN_BRIGHTNESS;
            m.brightness_max = RakkPirahPlus_MAX_BRIGHTNESS;
            m.brightness = m.brightness_max;
        }

        modes.push_back(m);
    }

    SetupZones();
    keepalive_thread_run = 1;
    keepalive_thread = new std::thread(&RGBController_RakkPirahPlusKeyboard::KeepaliveThread, this);
}

RGBController_RakkPirahPlusKeyboard::~RGBController_RakkPirahPlusKeyboard()
{
    keepalive_thread_run = 0;
    keepalive_thread->join();
    delete keepalive_thread;
    delete controller;
}

void RGBController_RakkPirahPlusKeyboard::SetupZones()
{

    RakkPirahPlus keyboard = matrix;

    unsigned int zone_size = 0;

    zone keyboard_zone;
    keyboard_zone.name               = ZONE_EN_KEYBOARD;
    keyboard_zone.type               = ZONE_TYPE_MATRIX;

    keyboard_zone.matrix_map         = new matrix_map_type;
    keyboard_zone.matrix_map->height = keyboard.height;
    keyboard_zone.matrix_map->width  = keyboard.width;

    keyboard_zone.matrix_map->map    = new unsigned int[keyboard.height * keyboard.width];

    for(unsigned int w = 0; w < keyboard.width; w++)
    {
        for(unsigned int h = 0; h < keyboard.height; h++)
        {
            unsigned int key = keyboard.matrix_map[h][w];
            keyboard_zone.matrix_map->map[h * keyboard.width + w] = key;

            if(key != NA)
            {
                led new_led;
                new_led.name = keyboard.led_names[key];
                leds.push_back(new_led);
                zone_size++;
            }
        }
    }

    keyboard_zone.leds_min           = zone_size;
    keyboard_zone.leds_max           = zone_size;
    keyboard_zone.leds_count         = zone_size;

    zones.push_back(keyboard_zone);

    SetupColors();
}

void RGBController_RakkPirahPlusKeyboard::ResizeZone(int /*zone*/, int /*new_size*/)
{
    /*---------------------------------------------------------*\
    | This device does not support resizing zones               |
    \*---------------------------------------------------------*/
}

void RGBController_RakkPirahPlusKeyboard::DeviceUpdateLEDs()
{
    if(modes[active_mode].value == DIRECT_MODE_VALUE)
    {
        last_update_time = std::chrono::steady_clock::now();
        controller->SendDirect(colors, matrix.positions_custom);
    }
    else
    {
        controller->SendCustom(colors, matrix.positions_custom);
    }
}

void RGBController_RakkPirahPlusKeyboard::UpdateZoneLEDs(int /*zone*/)
{
    controller->UpdateMode(modes, active_mode);
}

void RGBController_RakkPirahPlusKeyboard::UpdateSingleLED(int led)
{
    UpdateZoneLEDs(led);
}

void RGBController_RakkPirahPlusKeyboard::DeviceUpdateMode()
{
    UpdateZoneLEDs(0);
}

void RGBController_RakkPirahPlusKeyboard::KeepaliveThread()
{
    while(keepalive_thread_run.load())
    {
        if(active_mode == 0)
        {
            if((std::chrono::steady_clock::now() - last_update_time) > std::chrono::milliseconds(2000))
            {
                UpdateLEDs();
            }
        }
        std::this_thread::sleep_for(10ms);
    }
}
