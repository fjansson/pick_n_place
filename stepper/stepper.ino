/* Pick-and-place machine jogging test
 * RAMPS 1.4 board + Arduino Mega 2560 
 *
 * Fredrik Jansson 2016
 */

#include <MultiStepper.h>
#include <AccelStepper.h>


#define X 0
#define Y 1
#define R1 2
#define R2 3

#define PULSE_LENGTH_uS 100

#define STEP 0
#define DIR 1
#define ENABLE 2
#define MIN 3
#define MAX 4

#define LED_PIN 13

#define N_AXES 4

/* Note: change R1 to Z to get endstop pins ?
 *
 */

                        // Step, Dir, En, Min, Max     
const uint8_t mapPin[N_AXES][5] = {
  { 54,  55, 38,  3,  2},    // X
  { 60,  61, 56, 14, 15},    // Y
  { 26,  28, 24, -1, -1},    // R1
  { 36,  34, 30, -1, -1},    // R2
//{ 46,  48, 62, 18, 19},    // Z  -- not used ATM?
};


// parameters for each axis: range(steps), max speed, max acceleration
long int range[N_AXES] = {1000, 1000,  200,  200};
float a_max[N_AXES]    = { 400,  400,  400,  400};
float v_max[N_AXES]    = { 200,  200,  200,  200};


AccelStepper steppers[] = {
  AccelStepper (AccelStepper::DRIVER, mapPin[0][STEP], mapPin[0][DIR]),
  AccelStepper (AccelStepper::DRIVER, mapPin[1][STEP], mapPin[1][DIR]),
  AccelStepper (AccelStepper::DRIVER, mapPin[2][STEP], mapPin[2][DIR]),
  AccelStepper (AccelStepper::DRIVER, mapPin[3][STEP], mapPin[3][DIR])
};
  
   
void setup()
{
  pinMode(LED_PIN, OUTPUT);

  int i;
  for (i = 0; i < N_AXES; i++)
    {
      steppers[i].setEnablePin(mapPin[i][ENABLE]);
      steppers[i].setPinsInverted(false,false,true); // enable pin is active low
      steppers[i].enableOutputs();                    // needed for the enable pin to take effect (?)	
    }
  
  // pinMode(62, OUTPUT);
  // digitalWrite(62, HIGH);
}

void loop()
{
  steppers[0].run();
  steppers[1].run();
  steppers[2].run();
  steppers[3].run();
}
