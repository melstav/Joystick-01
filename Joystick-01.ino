/*************************************
 * This program implements a joystick controller using  a BluePill board (STM32F103C8).
 * It is written for the stm32duino core and the USBComposite library.
 * 
 * The initial joystick used for development is a 15th Anniversary Street Fighter Arcade Stick.
 * This stick features a standard digital 8-way joystick and has 8 buttons plus the start and select buttons, for a grand total of 10 buttons.
 * It originally included an interface for both the PlayStation 2 and original XBox, which will be replaced by the BluePill.
 */

/*************
 * Build History:
 * --------------
 * Original build: 20210430 BLC
 * Using the Arduino core provided by http://dan.drown.org/stm32duino/package_STM32duino_index.json (currently ver 2021.03.18)
 * and the built-in USBComposite library.
 */

#define JS_MAXVAL 1023
#define JS_MINVAL 0
#define JS_CENTER ((JS_MINVAL + JS_MAXVAL + 1) >> 1)

/*
 * Pin Definitions.
 * 
 * We're going to try keeping all of the pins used on one side of the board.
 */
#define PIN_JS_UP PB15
#define PIN_JS_DN PB12

#define PIN_JS_LT PB13
#define PIN_JS_RT PB14

/** PA11 and PA12 are USB D- & D+ **/
/** Skip over A10 also **/
/** Original code put BT_02 on PA10 instead of A15 **/
#define PIN_BT_01 PB6  /* X  */
#define PIN_BT_02 PB4  /* R1 */ 
#define PIN_BT_03 PB11  /* Y  */ 
#define PIN_BT_04 PB8  /* L1 */
#define PIN_BT_05 PB7  /* A  */
#define PIN_BT_06 PB5  /* B  */
#define PIN_BT_07 PA15 /* R2 */
#define PIN_BT_08 PB9  /* R1 */
#define PIN_BT_09 PA9  /* START */ 
#define PIN_BT_10 PA8  /* SELECT */

const int js_pins[] = {
 PIN_JS_UP, PIN_JS_DN, PIN_JS_LT, PIN_JS_RT
};
 
const int button_pins[] = {
 PIN_BT_01, PIN_BT_02, PIN_BT_03, PIN_BT_04, PIN_BT_05, 
 PIN_BT_06, PIN_BT_07, PIN_BT_08, PIN_BT_09, PIN_BT_10
};

#include <USBComposite.h>

// Instantiate the HID profile and create a Joystick endpoint
USBHID HID;
HIDJoystick Joystick(HID);

void setup() {
  int i = sizeof(js_pins)/sizeof(*js_pins);
  while (i--) {
    pinMode(js_pins[i], INPUT_PULLUP);  
  }
  i = sizeof(button_pins)/sizeof(*button_pins);
  while (i--) {
    pinMode(button_pins[i], INPUT_PULLUP);  
  }

  // Start the USB endpoint(s) and wait for it/them to be ready.
  HID.begin(HID_JOYSTICK);
  while (!USBComposite);
  
  // Inform the Joystick class that we will manually send reports when we are ready.
  Joystick.setManualReportMode(true);
}

void loop() {
  // Joystick Y Axis
  if (digitalRead(PIN_JS_UP) == LOW) {
    Joystick.Y(JS_MAXVAL);
  }
  else if (digitalRead(PIN_JS_DN) == LOW) {
    Joystick.Y(JS_MINVAL);
  }
  else {
    Joystick.Y(JS_CENTER);
  }

  // Joystick X Axis
  if (digitalRead(PIN_JS_RT) == LOW) {
    Joystick.X(JS_MAXVAL);
  }
  else if (digitalRead(PIN_JS_LT) == LOW) {
    Joystick.X(JS_MINVAL);
  }
  else {
    Joystick.X(JS_CENTER);
  }

  // Buttons
  int i = sizeof(button_pins)/sizeof(*button_pins);
  while (i--) {
    Joystick.button(i+1, (digitalRead(button_pins[i]) == LOW)?1:0);
  }

  // Send the report.
  Joystick.send();
}
