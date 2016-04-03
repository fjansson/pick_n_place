#define N_AXES 4

int RX_i = 0;
long RX_data[N_AXES];
short int RX_sign;
char RX_command = '\0';


void setup() {
  // put your setup code here, to run once:
Serial.begin (115200);
}

void processIncomingByte(char c)
{
  if (isdigit (c))
    {
      RX_data[RX_i] *= 10;
      RX_data[RX_i] += c - '0';
    }  // end of digit
  else 
    {
      // set the new state, if we recognize it
      switch (c)
  {
  case 'V':
    RX_command = c;
    RX_i = 0;
    RX_sign = 1;

    for (int i = 0; i < N_AXES; i++)
      RX_data[RX_i] = 0;
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
    
    for (int i = 0; i < N_AXES; i++)
      {
        Serial.write(RX_data[0]);
        Serial.write(' ');
      }
    Serial.write('\n');
    
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
  if (n % 1000 == 0)
   Serial.write('*');
}
