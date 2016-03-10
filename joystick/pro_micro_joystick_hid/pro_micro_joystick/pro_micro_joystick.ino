/* Analog joystick to HID Gamepad
 * Hacked for use with a Game Port Joystick by Fredrik Jansson 2016
 * 
 * using HID-Project library, https://github.com/NicoHood/HID
 * parts from  https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/example-2-hid-mouse-and-keyboard
 * hat switches: https://davenunez.wordpress.com/2014/02/22/reading-hat-switches-on-pc-gameport-joysticks/
 *               http://www.descent2.com/sickone/analogjoystick.html
 * 
 * linearization: http://www.built-to-spec.com/blog/2009/09/10/using-a-pc-joystick-with-the-arduino/
 * 
 * Connections:
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

const int xPin = A0;  // Analog output of horizontal joystick pin
const int yPin = A1;  // Analog output of vertical joystick pin
const int y2pin   = A2;  // Second y axis, used for hat.

int button1 = 9;  
int button2 = 8;  

int16_t x0, y0;  // Stores the initial value of each axis, usually around 512
float x0f, y0f;  // scaled, linearized float values of center position

int16_t x, y;    // Stores current analog output of each axis

const float Scale = 1023.0*55000; //1023 is the range of the ADC. 55000 is chosen for a nice range of output values.
const float alpha = .1;  // constant for rolling average filter. smaller number -> slower filter.

float x_filter = 0, y_filter = 0;

void setup() 
{
  pinMode(xPin, INPUT);  // Set both analog pins as inputs
  pinMode(yPin, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  digitalWrite(button1, HIGH);  // Pull-ups on
  digitalWrite(button2, HIGH);  
  
  delay(1000);  // short delay to let outputs settle
  
  x0 = analogRead(xPin);  // get the initial values
  y0 = analogRead(yPin);  // Joystick should be in neutral position when reading these
  
  x0f = Scale/x0;
  y0f = Scale/y0;

  // Sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

void loop() 
{
  // Analog axes 
  x =  analogRead(xPin);
  y =  analogRead(yPin);

  /* Simply return the raw ADC values, scaled.
  y = y - y0;
  x = x0 - x;
  
  Gamepad.xAxis(x<<7);
  Gamepad.yAxis(y<<7);
  */

  // convert to a linear scale
  float xf =  Scale/x - x0f;  
  float yf = -Scale/y + y0f;

  // rolling average for filtering
  x_filter = alpha * xf + (1-alpha) * x_filter;
  y_filter = alpha * yf + (1-alpha) * y_filter;

  Gamepad.xAxis((int)x_filter);
  Gamepad.yAxis((int)y_filter);
  
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

  // the thresholds are half-way between the y2-values observed when using a 100K resistor for the voltage divider.
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
}
