/* Analog joystick to HID Gamepad
 * Hacked for use with a Game Port Joystick by Fredrik Jansson 2016
 * 
 * using HID-Project library, https://github.com/NicoHood/HID
 * parts from  https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/example-2-hid-mouse-and-keyboard
 * hat switches: https://davenunez.wordpress.com/2014/02/22/reading-hat-switches-on-pc-gameport-joysticks/
 *               http://www.descent2.com/sickone/analogjoystick.html
 * 
 * Connections:
 * 
 * 
 *
 * Game port     Pro Micro
 * 1   Vcc          Vcc
 * 2   Button1       9
 * 3   X axis       A0   ---100K--- GND  
 * 4   GND          GND
 * 5   
 * 6   Y axis       A1   ---100K--- GND
 * 7   Button2       8
 * 8
 * 9
 * 10  Button4 
 * 11  X2 - unused??
 * 12
 * 13  Y2 - for hat  A2   ---100K--- GND
 * 14  Button3
 */
/*
  Copyright (c) 2014-2015 NicoHood
  See the readme for credit to other people.

  Gamepad example
  Press a button and demonstrate Gamepad actions

  You can also use Gamepad1,2,3 and 4 as single report.
  This will use 1 endpoint for each gamepad.

  See HID Project documentation for more infos
  https://github.com/NicoHood/HID/wiki/Gamepad-API
*/

#include "HID-Project.h"

const int horzPin = A0;  // Analog output of horizontal joystick pin
const int vertPin = A1;  // Analog output of vertical joystick pin
const int y2pin   = A2;  // Second y axis, used for hat.

int button1 = 9;  
int button2 = 8;  

int16_t x0, y0;  // Stores the initial value of each axis, usually around 512
float x0f, y0f;  // scaled, linearized float values of center position

int16_t x, y;    // Stores current analog output of each axis


const int pinLed = LED_BUILTIN;
const int pinButton = 9;

const float Scale = 1023.0*55000; //1023 is the range of the ADC. 55000 is chosen for a nice range of output values.


void setup() 
{
  pinMode(horzPin, INPUT);  // Set both analog pins as inputs
  pinMode(vertPin, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  digitalWrite(button1, HIGH);  // Pull button select pin high
  digitalWrite(button2, HIGH);  // Pull button select pin high
  
  delay(1000);  // short delay to let outputs settle
  y0 = analogRead(vertPin);  // get the initial values
  x0 = analogRead(horzPin);  // Joystick should be in neutral position when reading these
 
  x0f = Scale/x0;
  y0f = Scale/y0;

  pinMode(pinLed, OUTPUT);
  pinMode(pinButton, INPUT_PULLUP);

  // Sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

void loop() 
{

  // Analog axes 
  y =  analogRead(vertPin);
  x =  analogRead(horzPin);

  /* Simply return the raw ADC values, scaled.
  y = y - y0;
  x = x0 - x;
  
  Gamepad.xAxis(x<<7);
  Gamepad.yAxis(y<<7);
  */

  float xf =  Scale/x - x0f;  
  float yf = -Scale/y + y0f;
  
  /*
  Serial.print(x);
  Serial.print(" ");
  Serial.print(y);
  Serial.print(" ");
  Serial.print(xf);
  Serial.print(" ");
  Serial.print(yf);
  Serial.print("\n"); */

  Gamepad.xAxis((int)xf);
  Gamepad.yAxis((int)yf);
  
  // Buttons
  uint32_t b=0;
  b |= !digitalRead(button1);
  b |= !digitalRead(button2) << 1;
  
  Gamepad.buttons(b);
  
  
  // Hat. Some (i.e. our) joystics encode the 4 hat switches with different analog values on axis y2.
  uint8_t dpad1 = GAMEPAD_DPAD_CENTERED;
  int hat = analogRead(y2pin);
  //Serial.print(hat);
  //Serial.print("\n");

  // dir   observed ADC
  // up    1014    threshold
  //             926
  // right  838
  //             775
  // down   712
  //             667
  // left   622
  //             586
  // center 550 

  if (hat > 775)
  {
    if (hat > 926)
      dpad1 = GAMEPAD_DPAD_UP;
    else
      dpad1 = GAMEPAD_DPAD_RIGHT;
  }
  else // <= 775
  {
    if (hat > 667)
      dpad1 = GAMEPAD_DPAD_DOWN;  
    else if (hat > 586)
      dpad1 = GAMEPAD_DPAD_LEFT;
  }
  
  Gamepad.dPad1(dpad1); 
   
    
  Gamepad.write();  
  delay(5);

  
  /*
  if (!digitalRead(pinButton)) {
    digitalWrite(pinLed, HIGH);

    // Press button 1-32
    static uint8_t count = 0;
    count++;
    if (count == 33) {
      Gamepad.releaseAll();
      count = 0;
    }
    else
      Gamepad.press(count);

    // Move x/y Axis to a new position (16bit)
    Gamepad.xAxis(random(0xFFFF));
    Gamepad.yAxis(random(0xFFFF));

    // Go through all dPad positions
    // values: 0-8 (0==centered)
    static uint8_t dpad1 = GAMEPAD_DPAD_CENTERED;
    Gamepad.dPad1(dpad1++);
    if (dpad1 > GAMEPAD_DPAD_UP_LEFT)
      dpad1 = GAMEPAD_DPAD_CENTERED;

    static int8_t dpad2 = GAMEPAD_DPAD_CENTERED;
    Gamepad.dPad2(dpad2--);
    if (dpad2 < GAMEPAD_DPAD_CENTERED)
      dpad2 = GAMEPAD_DPAD_UP_LEFT;

    // Functions above only set the values.
    // This writes the report to the host.
    Gamepad.write();

    // Simple debounce
    delay(300);
    digitalWrite(pinLed, LOW);
  }*/
}
