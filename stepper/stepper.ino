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
const int8_t mapPin[N_AXES][5] = {
  { 54,  55, 38,  3,  2},    // X
  { 60,  61, 56, 14, 15},    // Y
  { 46,  48, 62, 18, 19},    // Z 
  { 26,  28, 24, -1, -1},    // R1
  //{ 36,  34, 30, -1, -1},    // R2
};


// parameters for each axis: range(steps), max speed, max acceleration
//long int range[N_AXES] = {1000, 1000,  200,  200};
//long int range[N_AXES] = {200*32*3.3, 200*32*3.3,  10000,  10000};
long int range[N_AXES] = {30000,  30000, 200*32*3.3*5, 200*32*3.3*5};
float a_max[N_AXES]    = { 4000,  4000,  4000,  4000};
float v_max[N_AXES]    = { 2000,  2000,  3000,  2000};


AccelStepper steppers[] = {
  AccelStepper (AccelStepper::DRIVER, mapPin[0][STEP], mapPin[0][DIR]),
  AccelStepper (AccelStepper::DRIVER, mapPin[1][STEP], mapPin[1][DIR]),
  AccelStepper (AccelStepper::DRIVER, mapPin[2][STEP], mapPin[2][DIR]),
  AccelStepper (AccelStepper::DRIVER, mapPin[3][STEP], mapPin[3][DIR])
};
  
int RX_i = 0;
long RX_data[N_AXES];
short int RX_sign;
char RX_command = '\0';

void setup()
{
  pinMode(LED_PIN, OUTPUT);

  int i;
  for (i = 0; i < N_AXES; i++)
    {
      steppers[i].setEnablePin(mapPin[i][ENABLE]);
      steppers[i].setPinsInverted(false,false,true);  // enable pin is active low
      steppers[i].setAcceleration(a_max[i]);
      RX_data[i] = 0;
      steppers[i].setMaxSpeed(0);                     // needed to get correct speeds later, when set again
      steppers[i].enableOutputs();                    // needed for the enable pin to take effect (?)
    }
  
//   pinMode(62, OUTPUT);
//   digitalWrite(62, HIGH);
 //pinMode(38, OUTPUT);
 //digitalWrite(38, HIGH);


  Serial.begin (115200);
//    Serial.begin (38400);

}

/*
 * for each axis, set the speed, then 
 * move towards either the positive or negative limit for that axis
 */
void setSpeed(long int *V)
{
  for (int i = 0; i < N_AXES; i++)
    {
      long int v = V[i];
      if (v > 0)
	{
	  steppers[i].setMaxSpeed(v);     // should be called before moveTo?
	  steppers[i].moveTo(range[i]);
	}
      else
	{
	  steppers[i].setMaxSpeed(-v);   // set a positive speed
	  steppers[i].moveTo(0);
	}
    }
}

/* Print received data for verification
 *
 */
void printData()
{
  for (int i = 0; i < N_AXES; i++)
    {
      Serial.print(' ');
      Serial.print(RX_data[i]);
    }
  Serial.print('\n');
}

/* Print the current position of all axes
 */
void reportPos()
{
  for (int i = 0; i < N_AXES; i++)
    {
      Serial.print(' ');
      Serial.print(steppers[i].currentPosition());
    }
  Serial.print('\n');
}

/* Stop all motors as fast as possible, without loosing position
 */
void stop()
{
  for (int i = 0; i < N_AXES; i++)
    steppers[i].stop();
}

/*
 * Receive a message in the following format
 *
 * V -100, 23, -45,13
 * ignore whitespace
 * numbers separated by ','
 * optional - sign at start of number
 *
 * currently: lazy parsing: accept '-' anywhere in the number
 * no error reporting
 * 
 *  
 * Commands:
 * V  - set velocities. up to 4 arguments.
 *
 * TODO: if an error is detected, don't execute the command
 */
void processIncomingByte(char c)
{
  if (isdigit (c))
    {
      RX_data[RX_i] *= 10;
      RX_data[RX_i] += c - '0';
    } 
  else 
    {
      // set the new state, if we recognize it
      switch (c)
	{
	case 'S':
	case 'W':
	case 'V':
	  // Serial.write("V\n");
	  RX_command = c;
	  RX_i = 0;
	  RX_sign = 1;
	  
	  for (int i = 0; i < N_AXES; i++)
	    RX_data[i] = 0;
	  break;
	  
	case ',':
	  RX_data[RX_i] *= RX_sign;
	  RX_sign = 1;
	  
	  if (RX_i < N_AXES-1)
	    RX_i++;
	  
	  //else - an error, too many ','
	  break;
	  
	case '-':
	  RX_sign = -1;
	  break;
	  
	case '\n':  //newline !
	  RX_data[RX_i] *= RX_sign;
	  
	  //Serial.print(RX_command);

	  switch(RX_command)
	    {
	    case 'W':
	      reportPos();
	      break;
	    case 'V':
	      setSpeed(RX_data);
	      //printData();
	      break;
	    case 'S':
	      stop();
	    default:
	      
	      break;        
	    }
  RX_command = '\0'; //prevent the same command from being run again by a repeated newline          
	  
	  break;
	case ' ':  // whitespace - ignore
	case '\t':
	case '\r':
	  break;
	  
	default:  
	  // an error - unknown character
	  break;
	} 
    } 
}


long int n;

void loop()
{
  if (Serial.available ())
    processIncomingByte (Serial.read ());

n++;
//if (n % 1000 == 0)
// Serial.write('*');

  steppers[0].run();
  steppers[1].run();
  steppers[2].run();
  steppers[3].run();
}
