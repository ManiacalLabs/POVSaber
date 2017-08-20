// LEDs
#define NUM_LEDS 144

#define DATA_PIN 11
#define CLOCK_PIN 13

#define LED_TYPE SK9822
#define COLOR_ORDER BGR

CRGB leds[NUM_LEDS];

// these are for the 0 - 100%
#define LED_LEVEL_STEP 10
uint8_t LED_LEVEL = 10;

// these are for 0 - 255
#define LED_LEVEL_MIN 0
#define LED_LEVEL_MAX 96


// Mode
#define MODE_MAIN 0
#define MODE_BRIGHTNESS 1
#define MODE_POV_FILES 2
#define MODE_POV 3
#define MODE_SABER_MENU 4
#define MODE_SABER 5
uint8_t _mode = MODE_MAIN;

const uint8_t main_menu_modes[] =
{
    MODE_BRIGHTNESS,
    MODE_POV_FILES,
    MODE_SABER_MENU
};

// Timing
long _last_frame = 0;
long _frame_delay = 0;

// Saber
bool _saber_shrink = false;
uint16_t _saber_step = 0;
CRGB _saber_base = CRGB(255, 0, 0);
uint8_t _saber_white_chance1 = 0;
uint8_t _saber_white_chance2 = 0;
uint8_t _saber_black_chance1 = 0;
uint8_t _saber_black_chance2 = 0;
#define SABER_FRAME_DELAY 3
#define SABER_FINAL_FRAME_DELAY 50

// Files
File img_file;
bool img_loaded = false;
File root;
uint32_t img_size = 0;
uint16_t img_rows = 0;
uint16_t img_fps = 0;
const uint16_t row_bytes = NUM_LEDS*3;
char row[row_bytes];

String _files[64];
uint8_t _file_count = 0;
String _cur_file_name;
