/* Logitech Wingman Extreme digital mode to USB HID Joystick 
 * by Fredrik Jansson 2016
 * 
 * using HID-Project library, https://github.com/NicoHood/HID
 * parts from  https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/example-2-hid-mouse-and-keyboard
 * 
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


void trigger()
{
  digitalWrite(xPin, LOW);  
  digitalWrite(yPin, LOW);   
  
  pinMode(xPin, OUTPUT);   // pull pins low
  pinMode(yPin, OUTPUT);

  delayMicroseconds(10);  // just guessing

  pinMode(xPin, INPUT);   // stop pulling pins
  pinMode(yPin, INPUT);
}

// put joystick in digital mode by triggering in a secret rythm
// from the Linux kernel adi driver.
void knock()
{
  short int i;
  short int seq[] = { 4, 2, 3, 10, 6, 11, 7, 9, 11, 0 };
  cli();
  for (i = 0; seq[i]; i++)
    {
      trigger();
      delay(seq[i]);
    }
    trigger(); // do we need this final trigger?
  sei();
}
void setup() 
{
  pinMode(xPin, INPUT);  // Set both analog pins as inputs
  pinMode(yPin, INPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
  digitalWrite(button1, HIGH);  // Pull-ups on
  digitalWrite(button2, HIGH);  

  delay(1000);  // short delay to let outputs settle

  // turn on the digital mode
  knock();
  delay(10);

  // Sends a clean report to the host. This is important on any Arduino type.
  Gamepad.begin();
}

/*receive n bits from the ADI*/
int getADIbits(int n)
{
  short int b1, b2;
  int val = 0;

  do {
  b1 = digitalRead(button1);
  b2 = digitalRead(button2);
  
  long int timeout = 100000;
  // wait for a change of state
  while(b1 == digitalRead(button1) && b2 == digitalRead(button2))
    {
      timeout--; // todo: timeout
      if (timeout == 0)
        break;
    }
if (timeout == 0)
  Serial.print("timeout\n");
    
  val <<= 1;
  if (b2 != digitalRead(button2))
      val++;  // button 2 changed, we got a 1
  
  } while (--n);
  return val;
}

short int hat = 0;
short int buttons = 0;
short int Taxis = 0;
short int Xaxis = 0;
short int Yaxis = 0;
  
void readADI()
{
  trigger();
  hat     = getADIbits(4);
  buttons = getADIbits(6);
  //Taxis   = getADIbits(8);
  //Xaxis   = getADIbits(8);
  //Yaxis   = getADIbits(8);  
  Serial.print(hat);
  Serial.print(buttons);
  Serial.print(Taxis);
  Serial.print(Xaxis);
  Serial.print(Yaxis);
  Serial.print('\n');
}

void loop() 
{
  readADI();
  /*
  // Analog axes 
  x =  analogRead(xPin);
  y =  analogRead(yPin);

  /* Simply return the raw ADC values, scaled.
  y = y - y0;
  x = x0 - x;
  
  Gamepad.xAxis(x<<7);
  Gamepad.yAxis(y<<7);
  */
/*
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
  */
  delay(1000);
}
