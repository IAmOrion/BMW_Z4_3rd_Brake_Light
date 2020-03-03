/*=========================================================================*/
/*                          Written by James Tanner                        */
/*                  https://www.facebook.com/TheMeroving1an                */
/*                                  IAmOrion                               */
/*                                Version: 1.5                             */
/*=========================================================================*/

#define VERSION "1.5"

/*=========================================================================*/
/*================================= SETTINGS ==============================*/
/*=========================================================================*/

/*=========================================================================*/
/*                                 USER CONFIG                             */
/*=========================================================================*/

boolean STARTUP_ANIMATION=true; // Set to false to disable the startup cylon effect
boolean COLOUR_CORRECTION=false; // Only valid for indicators - uses a Colour GAMMA Shift
boolean INITIAL_TEST=false; // Set to true to do a simple LED test.  This means that all LEDs will be lit RED at startup and remain lit.
// NOTHING ELSE WILL HAPPEN IF THIS IS SET TO TRUE! ONCE YOU CONFIRM ALL LEDS WORK - SET THIS TO FALSE AND RE-UPLOAD THE SKETCH

boolean PRE_FACELIFT = false;

const int CUSTOM_FUNCTION_1 = 1;
const int CUSTOM_FUNCTION_2 = 2;
const int CUSTOM_FUNCTION_3 = 5;
const int KNIGHT_RIDER_SPEED = 40; //default = 30 (miliseconds per LED)

/* Custom Effects List: */
/*
 * 1 = Knight Rider Scanner - Red
 * 2 = Rainbow Cycle - Colour cycle
 * 3 = Theater Chase - 
 * 4 = Colour Wipe - Random Colour
 * 5 = Police Flash - Blue / Red
 * 6 = Police Flash - Blue / Blue
*/

uint8_t flashrate = 165; //default = 165; //time in ms that indicators flash for

//#define BLUETOOTH_NANO //Comment OUT this line if you're NOT using a BLE Nano

/*=========================================================================*/
/*                   FAKE FORCE BRAKE LIGHT SETTINGS                       */
/*=========================================================================*/

boolean FORCE_BRAKE_ENABLED=false; // Set to false to simply turn red, set to True to flash x times then stay on
const int FORCE_BRAKE_TIMES_FLASH=4; // Default = 3
const int FORCE_BRAKE_FLASH_RATE=100; // Default = 100 (time in miliseconds (ms))

/*=========================================================================*/
/*         Pin Assignments (DO NOT CHANGE THESE IF USING MY PCB)           */
/*=========================================================================*/

const int brake = A0, reverse = A1, left = A2, right = A3;
const int custom1 = A4, custom2 = A5;
const int indicators_enabled = 10, indicator_mode_select = 2;

/*=========================================================================*/
/*                               LED Pixel Setup                           */
/*=========================================================================*/

const int pixel_pin = 3, number_of_pixels = 20, LED_BRIGHTNESS=255;
//const int pixel_pin_2 = 8, number_of_pixels_2 = 20, LED_BRIGHTNESS_2=255;

uint8_t pixelrate = (flashrate/(number_of_pixels/2));

/*=========================================================================*/
/*                      Serial Debugging / Testing Settings                */
/*=========================================================================*/

#define SERIAL_DEBUG
//#define SERIAL_DEBUG_VOLTAGES
//#define SERIAL_DEBUG_ACTIONS
//#define WAIT_FOR_SERIAL

#define BT_BAUD_RATE   9600 //KeyWish BLE Nano works best at 9600, so do not change
#define BAUD_RATE      115200

/*=========================================================================*/
/*================================ VARIABLES ==============================*/
/*=========================================================================*/
/*===== DO NOT CHANGE ANYTHING BELOW UNLESS YOU KNOW WHAT YOU'RE DOING ====*/
/*=========================================================================*/

boolean INDICATORS, LAST_INDICATORS;
float brake_voltage=0.00, reverse_voltage=0.00, left_voltage=0.00, right_voltage=0.00;
float custom1_voltage=0.00, custom2_voltage=0.00, min_voltage = 4.00, brake_min_voltage = 11.00;
boolean BRAKING = false, REVERSING = false, TURNING_LEFT = false, TURNING_RIGHT = false;
boolean CUSTOM1 = false, CUSTOM2 = false, ACTIVE=false, TEST_MODE=false;
int MODE = 0, LAST_MODE=MODE, INDICATOR_MODE=1, COUNTER=0, CompleteCount = 0;
boolean SERIAL_DONE=true; String inString = "";

int left_off_couter=0, right_off_counter=0;
const int counter_max = 10;

float vout = 0.0; int value = 0;
float R1 = 30000.0; // resistance of R1 (30K)
float R2 = 6800.0; // resistance of R2 (6.8K)

int serial_i = -1; byte serial_b;
long previousMillis = 0; long interval = flashrate;

#ifdef BLUETOOTH_NANO
  boolean BT_BRAKING = false, BT_REVERSING = false, BT_TURNING_LEFT = false, BT_TURNING_RIGHT = false;
  boolean BT_HAZARDS = false, BT_CUSTOM1 = false, BT_CUSTOM2 = false, BT_CUSTOM3 = false;
#endif

/*=========================================================================*/
/*                           NeoPixel Preset Colours                       */
/*=========================================================================*/

#define OFF          0,0,0
#define WHITE_       255,255,255
#define RED          255,0,0
#define BLUE         0,0,255
#define GREEN        0,255,0
#define ORANGE       255,140,0

uint8_t cRed = 255;     // RED colour value in standard RGB format.
uint8_t cGreen = 147;   // GREEN colour value in standard RGB format.
uint8_t cBlue = 0;      // BLUE colour value in standard RGB format.

uint8_t cRed_b = 255;     // RED colour value in standard RGB format.
uint8_t cGreen_b = 80;    // GREEN colour value in standard RGB format.
uint8_t cBlue_b = 0;      // BLUE colour value in standard RGB format.


const uint8_t PROGMEM gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 
};
