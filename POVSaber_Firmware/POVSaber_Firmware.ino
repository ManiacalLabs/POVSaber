#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "FastLED.h"
#include "Bounce2.h"
FASTLED_USING_NAMESPACE

/****************************
All Firmware options go here!
****************************/
// Uncomment below line if EEPROM available on your board
// #define USE_EEPROM

// If not using EEPROM, you can specify device ID here
#ifndef USE_EEPROM
    const uint8_t deviceID = 0;
#endif

// How many leds in your strip?
#define NUM_LEDS 144

#define DATA_PIN 11
#define CLOCK_PIN 13

#define LED_TYPE SK9822
#define COLOR_ORDER BGR

CRGB leds[NUM_LEDS]; // Define the array of leds
/***************************
End Firmware options
***************************/

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
    // delay(2000);
    // display.clearDisplay();
    // display.setTextSize(1);
    // display.setTextColor(WHITE);
    // display.setCursor(0,0);
    // display.println("POV Saber");
    // display.setTextColor(BLACK, WHITE); // 'inverted' text
    // display.println("Animation");
    // display.display();

}

void displn(String txt, byte row, boolean inverted){
    display.setCursor(0, row * 8);
    if(inverted) display.setTextColor(BLACK, WHITE);
    else display.setTextColor(WHITE);
    display.print(txt);
}

void displn(String txt, byte row){
    displn(txt, row, false);
}

void display_menu(String title, String items[], byte s){
    static byte i = 0;
    displn("File List", 0, true);
    for(i=0; i<3; i++){
        displn((s==i ? ">":" ") + items[i], i+1);
    }
    display.display();
}

// File Stuff
File img_file;
File root;
uint32_t img_size = 0;
uint16_t img_rows = 0;
const uint16_t row_bytes = NUM_LEDS*3;
char row[row_bytes];

String _files[64];
uint8_t _file_count = 0;

void find_files(){
    File root = SD.open("/");
    while(true){
        File f = root.openNextFile();
        if(!f){ f.close(); break; Serial.println("No More");} //no more files, exit
        if(!f.isDirectory()){ //it's a file!
            _files[_file_count] = String(f.name());
        }
        _file_count++;
        f.close();
    }
    root.close();
}

void init_sdcard(){
    // Serial.print("Initializing SD card...");
    if (!SD.begin(BUILTIN_SDCARD)) {
      // Serial.println("initialization failed!");
      return;
    }
    else {
      // Serial.println("initialization done.");
    }
}

void display_files(){
    static byte i = 0;
    display.clearDisplay();
    display.setCursor(0,0);
    display.setTextColor(WHITE);
    for(i=0; i<4; i++){
        display.println(_files[i]);
    }
    display.display();
}

inline void load_file(){
    img_file = SD.open("ml_logo", FILE_READ);
    img_size = img_file.size();
    if(img_size % row_bytes != 0){
        // Serial.println("Image wrong size!");
    }
}

void load_next_row(){
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

// LED Stuff
uint16_t numLEDs = NUM_LEDS;
uint8_t bytesPerPixel = 3;

inline void setupFastLED()
{
    FastLED.addLeds<LED_TYPE, DATA_PIN, CLOCK_PIN, COLOR_ORDER>(leds, NUM_LEDS);

    // FastLED.setBrightness(32);
    FastLED.clear();
    FastLED.show();
}

// Btn Handlers

void doA(){
    Serial.println("A!");
}

void doB(){
    Serial.println("B!");
}

void doC(){
    Serial.println("C!");
}

// Button Stuff
#define BTN_A 0
#define BTN_B 1
#define BTN_C 2
#define BTN_INTERVAL 5

typedef struct __attribute__((__packed__))
{
	boolean A;
    boolean B;
    boolean C;
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
    static boolean a, b, c;
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


void setup()
{
    // Full USB 1.1 speed, assuming your chip has native USB
    Serial.begin(12000000);
    Serial.setTimeout(1000);

    setup_btns();

    init_sdcard();

    // Serial.println("TESTING!!!!!");
    // delay(2000);
    // Serial.println("Hello World!");

    // setupFastLED();
    setup_display();

    find_files();
    String items[3] = {
        _files[0], _files[1], _files[2]
    };
    display_menu("File List", items, 0);
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

void loop()
{
    // getData();
    // FastLED.delay(0);
    // if(false){
    //     load_next_row();
    //     FastLED.show();
    //     FastLED.delay(10);
    // }

    read_btns();

    // if(btns.A){ Serial.println("A"); }
    // if(btns.B){ Serial.println("B"); }
    // if(btns.C){ Serial.println("C"); }
}
