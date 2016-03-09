#include <HID.h>
#include <Mouse.h>

/* Analog joystick to HID mouse example for the Pro Micro board
 * from  https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/example-2-hid-mouse-and-keyboard
 * 
 * Hacked for use with a Game Port Joystick by Fredrik Jansson 2016
 * 
 * Connections:
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
 * 13  Y2 - for hat
 * 14  Button3
 */

/* HID Joystick Mouse Example
   by: Jim Lindblom
   date: 1/12/2012
   license: MIT License - Feel free to use this code for any purpose.
   No restrictions. Just keep this license if you go on to use this
   code in your future endeavors! Reuse and share.

   This is very simplistic code that allows you to turn the 
   SparkFun Thumb Joystick (http://www.sparkfun.com/products/9032)
   into an HID Mouse. The select button on the joystick is set up
   as the mouse left click. 
 */

 
int horzPin = A0;  // Analog output of horizontal joystick pin
int vertPin = A1;  // Analog output of vertical joystick pin
int button1 = 9;  
int button2 = 8;  

int vertZero, horzZero;  // Stores the initial value of each axis, usually around 512
int vertValue, horzValue;  // Stores current analog output of each axis
const int sensitivity = 20;  // Higher sensitivity value = slower mouse, should be <= about 500
int button1_flag = 0;
int button2_flag = 0;

void setup()
{
  pinMode(horzPin, INPUT);  // Set both analog pins as inputs
  pinMode(vertPin, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  digitalWrite(button1, HIGH);  // Pull button select pin high
  digitalWrite(button2, HIGH);  // Pull button select pin high
  
  delay(1000);  // short delay to let outputs settle
  vertZero = analogRead(vertPin);  // get the initial values
  horzZero = analogRead(horzPin);  // Joystick should be in neutral position when reading these

}

void loop()
{
  vertValue = -(analogRead(vertPin) - vertZero);  // read vertical offset
  horzValue = -(analogRead(horzPin) - horzZero);  // read horizontal offset

  if (vertValue != 0)
    Mouse.move(0, vertValue/sensitivity, 0);  // move mouse on y axis
  if (horzValue != 0)
    Mouse.move(horzValue/sensitivity, 0, 0);  // move mouse on x axis

  if ((digitalRead(button1) == 0) && (!button1_flag))  // if the joystick button is pressed
  {
    button1_flag = 1;
    Mouse.press(MOUSE_LEFT);  // click the left button down
  }
  else if ((digitalRead(button1))&&(button1_flag)) // if the joystick button is not pressed
  {
    button1_flag = 0;
    Mouse.release(MOUSE_LEFT);  // release the left button
  }

   if ((digitalRead(button2) == 0) && (!button2_flag))  // if the joystick button is pressed
  {
    button2_flag = 1;
    Mouse.press(MOUSE_RIGHT);  // click the left button down
  }
  else if ((digitalRead(button2))&&(button2_flag)) // if the joystick button is not pressed
  {
    button2_flag = 0;
    Mouse.release(MOUSE_RIGHT);  // release the left button
  }
  
}



