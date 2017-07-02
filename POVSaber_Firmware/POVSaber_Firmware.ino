#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FastLED.h"
#include "Bounce2.h"
#include "globals.h"
#include "menus.h"
FASTLED_USING_NAMESPACE

#ifndef USE_EEPROM
    const uint8_t deviceID = 0;
#endif

#ifdef USE_EEPROM
    #include <EEPROM.h>
#endif

#define FIRMWARE_VER 3

namespace CMDTYPE
{
    enum CMDTYPE
    {
        SETUP_DATA = 1,
        PIXEL_DATA = 2,
        BRIGHTNESS = 3,
        GETID      = 4,
        SETID      = 5,
        GETVER       = 6
    };
}

namespace RETURN_CODES
{
    enum RETURN_CODES
    {
        SUCCESS = 255,
        REBOOT = 42,
        ERROR = 0,
        ERROR_SIZE = 1,
        ERROR_UNSUPPORTED = 2,
        ERROR_PIXEL_COUNT = 3,
        ERROR_BAD_CMD = 4,
    };
}

typedef struct __attribute__((__packed__))
{
	uint8_t type;
	uint16_t pixelCount;
	uint8_t spiSpeed;
} config_t;

// Display Stuff
#define OLED_RESET 20
//SCL - 19
//SDA - 20
Adafruit_SSD1306 display(OLED_RESET);

void setup_display(){
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.clearDisplay();
    display.setTextSize(1);
    display.display();
}

void showln(String txt, bool inverted, uint8_t x, uint8_t y){
    display.clearDisplay();
    display.setCursor(x, y);
    if(inverted) display.setTextColor(BLACK, WHITE);
    else display.setTextColor(WHITE);
    display.print(txt);
    display.display();
}

void showln(String txt, bool inverted){
    showln(txt, inverted, 0, 12);
}

void showln(String txt){
    showln(txt, false);
}

void displn(String txt, byte row, bool inverted){
    display.setCursor(0, row * 8);
    if(inverted) display.setTextColor(BLACK, WHITE);
    else display.setTextColor(WHITE);
    display.print(txt);
}

void displn(String txt, byte row){
    displn(txt, row, false);
}

void display_menu(String title, String items[], uint8_t len, byte s){
    static int8_t i;
    static int8_t entries[3];
    static uint8_t max;
    max = 3;
    if(len >= max){
        entries[0] = s - 1;
        entries[1] = s;
        entries[2] = s + 1;
        if(entries[0] < 0) entries[0] = len - 1;
        if(entries[2] > (len - 1)) entries[2] = 0;
        s = 1; // set back to center entry
    }
    else{
        entries[0] = 0;
        entries[1] = 1;
        entries[2] = 2;
        max = len;
    }

    displn(title, 0, true);

    for(i=0; i<max; i++){
        displn((s==i ? ">":" ") + items[entries[i]], i+1);
    }

    display.display();
}

// File Stuff

void find_files(){
    showln("Loading files...");
    File root = SD.open("/");
    while(true){
        File f = root.openNextFile();
        if(!f){ f.close(); break;} //no more files, exit
        if(!f.isDirectory()){ //it's a file!
            _files[_file_count] = String(f.name());
            showln(f.name());
            _file_count++;
        }
        f.close();
    }
    root.close();
}

bool init_sdcard(){
    showln("Loading SD card...");
    if (!SD.begin(BUILTIN_SDCARD)) {
      // Serial.println("initialization failed!");
      showln("   SD Load FAIL :(   ", true);
      return false;
    }
    else {
      showln("   SD Load Success!   ", true);
    }

    return true;
}

void load_file(String filename){
    img_loaded = false;
    _cur_file_name = "";
    img_file = SD.open(filename.c_str(), FILE_READ);
    img_size = img_file.size();
    if(img_size % row_bytes != 0){
        showln("File wrong size!");
        delay(2000);
    }
    else{
        uint8_t len = filename.length();
        String ext = filename.substring(len - 3);
        img_fps = ext.toInt();
        if(img_fps == 0) img_fps = 30;
        _frame_delay = 1000 / img_fps;
        _cur_file_name = filename;
        img_loaded = true;
        _last_frame = 0;
    }
}

void load_next_row(){
    if(img_loaded){
        static uint16_t bytes_read = 0;
        bytes_read = img_file.read(((char*)&leds), row_bytes);
        if(bytes_read != row_bytes){
            // File is wrong size, but gracefully fail
            img_file.seek(0); //return to beginning
            return;
        }
        else if(img_file.position() >= img_size){
            img_file.seek(0);
        }
    }
}

// LED Stuff
uint16_t numLEDs = NUM_LEDS;
uint8_t bytesPerPixel = 3;

inline void setupFastLED()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);

    set_brightness();
    FastLED.clear();
    FastLED.show();
}

void set_brightness(){
    FastLED.setBrightness(map(LED_LEVEL, 0, 100, LED_LEVEL_MIN, LED_LEVEL_MAX));
}

// Btn Handlers
void doA(){
    handle_menu_select();
}

void doB(){
    switch (_mode) {
        case MODE_MAIN:
        case MODE_POV_FILES:
        case MODE_SABER_MENU:
            _menu_item++;
            break;
    }
    display_cur_menu();
}

void doC(){
    handle_menu_back();
}

// Button Stuff
#define BTN_A 0
#define BTN_B 1
#define BTN_C 2
#define BTN_INTERVAL 5

typedef struct __attribute__((__packed__))
{
	bool A;
    bool B;
    bool C;
} btns_t;

btns_t btns;

Bounce btn_a = Bounce();
Bounce btn_b = Bounce();
Bounce btn_c = Bounce();

inline void setup_btns(){
    pinMode(BTN_A, INPUT_PULLUP);
    pinMode(BTN_B, INPUT_PULLUP);
    pinMode(BTN_C, INPUT_PULLUP);

    btn_a.attach(BTN_A);
    btn_a.interval(BTN_INTERVAL);

    btn_b.attach(BTN_B);
    btn_b.interval(BTN_INTERVAL);

    btn_c.attach(BTN_C);
    btn_c.interval(BTN_INTERVAL);
}

inline void read_btns(){
    static bool a, b, c;
    btn_a.update();
    btn_b.update();
    btn_c.update();

    a = !btn_a.read();
    if(a && !btns.A) { doA(); btns.A = true; }
    else if(!a && btns.A){ btns.A = false; }

    b = !btn_b.read();
    if(b && !btns.B) { doB(); btns.B = true; }
    else if(!b && btns.B){ btns.B = false; }

    c = !btn_c.read();
    if(c && !btns.C) { doC(); btns.C = true; }
    else if(!c && btns.C){ btns.C = false; }
}

// Menu Stuff
void display_main_menu(){
    if(_menu_item>=MENU_MAIN_LEN) _menu_item = 0;
    display_menu(MENU_MAIN_TITLE, _menu_main, MENU_MAIN_LEN, _menu_item);
}

void display_pov_file_menu(){
    if(_menu_item>=_file_count) _menu_item = 0;
    display_menu(MENU_FILES_TITLE, _files, _file_count, _menu_item);
}

void display_pov_menu(){
    displn("     Showing POV     ", 0, true);
    displn(_cur_file_name, 2, false);
    displn(String(img_fps) + " FPS", 3, false);
    display.display();
}

void display_bright_menu(){
    displn("     Brightness     ", 0, true);
    display.setTextColor(WHITE);
    display.setTextSize(2);
    display.setCursor(0, 12);
    display.print("   ");
    display.print(LED_LEVEL);
    display.print("%");
    display.setTextSize(1);
    display.display();
}

void display_saber_list_menu(){
    if(_menu_item>=MENU_SABER_LEN) _menu_item = 0;
    display_menu(MENU_SABER_TITLE, _menu_sabers, MENU_SABER_LEN, _menu_item);
}

void display_saber_menu(){
    displn("      Saber     ", 0, true);
    displn(_menu_sabers[_menu_item], 2, false);
    display.display();
}

void display_cur_menu(){
    display.clearDisplay();
    switch(_mode){
        case MODE_MAIN:
            display_main_menu();
            break;
        case MODE_POV_FILES:
            display_pov_file_menu();
            break;
        case MODE_POV:
            display_pov_menu();
            break;
        case MODE_BRIGHTNESS:
            display_bright_menu();
            break;
        case MODE_SABER_MENU:
            display_saber_list_menu();
            break;
        case MODE_SABER:
            display_saber_menu();
            break;
        default:
            showln("Invalid Menu");
    }
}

void handle_menu_select(){
    switch(_mode){
        case MODE_MAIN:
            _mode = main_menu_modes[_menu_item];
            _menu_item = 0;
            break;
        case MODE_POV_FILES:
            load_file(_files[_menu_item]);
            if(img_loaded)
                _mode = MODE_POV;
            break;
        case MODE_POV:
            img_loaded = false;
            _mode = MODE_POV_FILES;
            break;
        case MODE_BRIGHTNESS:
            LED_LEVEL += LED_LEVEL_STEP;
            if(LED_LEVEL > 100) LED_LEVEL = LED_LEVEL_STEP;
            set_brightness();
            break;
        case MODE_SABER_MENU:
            _last_frame = 0;
            _frame_delay = SABER_FRAME_DELAY;
            _saber_step = 0;
            _saber_shrink = false;
            _mode = MODE_SABER;
            setup_saber();
            break;
        case MODE_SABER:
            _saber_shrink = true;
            _frame_delay = SABER_FRAME_DELAY;
            break;

    }
    display_cur_menu();
}

void handle_menu_back(){
    switch (_mode) {
        case MODE_POV_FILES:
        case MODE_SABER_MENU:
        case MODE_BRIGHTNESS:
            _menu_item = 0;
            _mode = MODE_MAIN;
            break;
    }
    display_cur_menu();
}

void setup()
{
    // Full USB 1.1 speed, assuming your chip has native USB
    Serial.begin(12000000);
    Serial.setTimeout(1000);

    setupFastLED();
    setup_display();
    if(!init_sdcard()) return;

    find_files();

    display_cur_menu();

    setup_btns();
}

#define EMPTYMAX 100
inline void getData()
{
    static char cmd = 0;
    static uint16_t size = 0;
    static uint16_t count = 0;
    static uint8_t emptyCount = 0;
    static size_t c = 0;
    static uint16_t packSize = numLEDs * bytesPerPixel;

    if (Serial.available())
    {
        cmd = Serial.read();
        size = 0;
        Serial.readBytes((char*)&size, 2);

        if (cmd == CMDTYPE::PIXEL_DATA)
        {
            count = 0;
            emptyCount = 0;

            if (size == packSize)
            {
                while (count < packSize)
                {
                    c = Serial.readBytes(((char*)&leds) + count, packSize - count);
                    if (c == 0)
                    {
                        emptyCount++;
                        if(emptyCount > EMPTYMAX) break;
                    }
                    else
                    {
                        emptyCount = 0;
                    }

                    count += c;
                }
            }

            uint8_t resp = RETURN_CODES::SUCCESS;
            if (count == packSize)
            {
                FastLED.show();
            }
            else
                resp = RETURN_CODES::ERROR_SIZE;

            Serial.write(resp);
        }
        else if(cmd == CMDTYPE::GETID)
        {
            #ifdef USE_EEPROM
                Serial.write(EEPROM.read(16));
            #else
                Serial.write(deviceID);
            #endif
        }
        else if(cmd == CMDTYPE::SETID)
        {
            if(size != 1)
            {
                Serial.write(RETURN_CODES::ERROR_SIZE);
            }
            else
            {
                uint8_t id = Serial.read();
                #ifdef USE_EEPROM
                    EEPROM.write(16, id);
                #endif
                Serial.write(RETURN_CODES::SUCCESS);
            }
        }
        else if (cmd == CMDTYPE::SETUP_DATA)
        {
            // for(int i=0; i<size; i++) Serial.read();

            uint8_t result = RETURN_CODES::SUCCESS;
            config_t temp;

            if (size != sizeof(config_t))
            {
                result = RETURN_CODES::ERROR_SIZE;
            }
            else
            {
                size_t read = Serial.readBytes((char*)&temp, sizeof(config_t));
                if (read != size)
                {
                    result = RETURN_CODES::ERROR_SIZE;
                }
                else
                {
                    if(temp.pixelCount / bytesPerPixel != NUM_LEDS)
                        result = RETURN_CODES::ERROR_PIXEL_COUNT;
                }
            }

            Serial.write(result);
        }
        else if (cmd == CMDTYPE::BRIGHTNESS)
        {
            uint8_t result = RETURN_CODES::SUCCESS;
            if (size != 1)
                result = RETURN_CODES::ERROR_SIZE;
            else
            {
                uint8_t brightness = 255;
                size_t read = Serial.readBytes((char*)&brightness, 1);
                if (read != size)
                    result = RETURN_CODES::ERROR_SIZE;
                else
                {
                    FastLED.setBrightness(brightness);
                }
            }

            Serial.write(result);
        }
        else if (cmd == CMDTYPE::GETVER)
        {
            Serial.write(RETURN_CODES::SUCCESS);
            Serial.write(FIRMWARE_VER);
        }
        else
        {
            Serial.write(RETURN_CODES::ERROR_BAD_CMD);
        }


        Serial.flush();
    }
}

void clear_strip(){
    FastLED.clear();
    FastLED.show();
}

inline void mode_pov(){
    if(img_loaded){
        if(millis() - _last_frame > _frame_delay){
            load_next_row();
            FastLED.show();
            _last_frame = millis();
        }
    }
    else{
        clear_strip();
    }
}

inline void mode_bright(){
    // fill_solid(leds, NUM_LEDS, CRGB::Red);
    fill_rainbow(leds, NUM_LEDS, 0, 3);
    FastLED.show();
}

void add_flash( fract8 chance, CRGB color)
{
  if( random8() < chance) {
    leds[ random16(NUM_LEDS) ] = color;
  }
}

inline void mode_saber(){
    if(millis() - _last_frame > _frame_delay){
        if(_saber_shrink){
            FastLED.clear();
            fill_solid(leds, _saber_step, _saber_base);
            _saber_step--;
            if(_saber_step <= 0){
                _mode = MODE_SABER_MENU;
                display_cur_menu();
            }
        }
        else{
            if(_saber_step < NUM_LEDS){
                FastLED.clear();
                fill_solid(leds, _saber_step, _saber_base);
                _saber_step++;
            }
            else{
                _frame_delay = SABER_FINAL_FRAME_DELAY;
                FastLED.clear();
                fill_solid(leds, NUM_LEDS, _saber_base);
                add_flash(_saber_black_chance1, {0, 0, 0});
                add_flash(_saber_black_chance2, {0, 0, 0});
                add_flash(_saber_white_chance1, {32, 32, 32});
                add_flash(_saber_white_chance2, {128, 128, 128});

            }
        }
        FastLED.show();
        _last_frame = millis();
    }
}

inline void setup_saber(){
    _saber_base = CRGB::Red;
    _saber_white_chance1 = 128;
    _saber_white_chance2 = 0;
    _saber_black_chance1 = 128;
    _saber_black_chance2 = 0;
    switch (_menu_item) {
        case 0: //Red
            _saber_base = CRGB::Red;
            break;
        case 1: //Orange
            _saber_base = CRGB::Orange;
            break;
        case 2: //Yellow
            _saber_base = CRGB::Yellow;
            break;
        case 3: //Green
            _saber_base = CRGB::Green;
            break;
        case 4: //Blue
            _saber_base = CRGB::Blue;
            break;
        case 5: //Purple
            _saber_base = CRGB::Purple;
            break;
        case 6: //Kylo
            _saber_base = CRGB::Red;
            _saber_white_chance1 = 255;
            _saber_white_chance2 = 255;
            _saber_black_chance1 = 255;
            _saber_black_chance2 = 255;
            break;
    }
}

void loop()
{
    // getData();
    // FastLED.delay(0);
    // if(img_loaded){
    //     load_next_row();
    //     FastLED.show();
    //     FastLED.delay(_frame_delay);
    // }
    // else{
    //     FastLED.clear();
    //     FastLED.show();
    // }

    switch (_mode) {
        case MODE_POV:
            mode_pov();
            break;
        case MODE_BRIGHTNESS:
            mode_bright();
            break;
        case MODE_SABER:
            mode_saber();
            break;
        default:
            clear_strip();
            break;
    }

    read_btns();
}
