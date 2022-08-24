#include <Stepper.h> // Stepper 

#define motorSteps 200     // Number of steps per revloution for the motors

//Pins for Stepper 1-4
#define motor1Pin1 13
#define motor1Pin2 12
#define motor2Pin1 11
#define motor2Pin2 10
#define motor3Pin1 9
#define motor3Pin2 8
#define motor4Pin1 7
#define motor4Pin2 6

//Length of MIDI values
#define tunelength 400

//Beats per minute
#define tempo 80

// 1/8th of the beat time in microseconds
const long beattime = 187500;

//PROGMEM stores the MIDI array tune is flash memory
const unsigned char tune[tunelength] PROGMEM = { 35  , 0 , 37  , 0 , 38 , 0 , 40 , 0 , 42  , 0 , 38 , 0 , 42  , 0 , 0, 0 , 41  , 0 , 37  , 0 , 41  , 0 , 0, 0 , 40 , 0 , 36  , 0 , 40  , 0, 0 , 0 , 35 , 0 , 37 , 0, 38 , 0, 40  , 0 , 42  , 0, 38 , 0 , 42 , 0 , 47  , 0 , 45 , 0 , 42 , 0 , 38  , 0 , 42 , 0 , 45  , 0 , 0 , 0, 0 , 0 , 0, 0 , 35  , 0 , 0 , 0 , 42 , 0 , 0 , 0 , 35  , 0 , 0 , 0 , 42  , 0 , 0 , 0 , 35  , 0 , 0, 0, 42 , 0, 0 , 0 , 35 , 0 , 0, 0 , 42 , 0, 0 , 0 , 35,  0, 0, 0, 42 , 0 , 0, 0 , 35  , 0 , 0 , 0 , 42  , 0 , 0 , 0 , 38 , 0 , 0, 0, 45  , 0 , 0, 0, 38,  0 , 0 , 0 , 45 , 0 , 0 , 0 , 42 , 0, 44  , 0 , 46,  0 , 47  , 0, 49  , 0 , 46 , 0 , 49  , 0, 0, 0 , 50  , 0 , 46 , 0 , 50,  0, 0, 0 , 49,  0 , 46,  0 , 49 , 0 , 0 , 0 , 42  , 0 , 44,  0 , 46,  0, 47  , 0 , 49 , 0 , 46,  0, 49 , 0, 0 , 0 , 50 , 0 , 46 , 0, 50 , 0 , 0 , 0 , 49,  0 , 0, 0, 0, 0 , 0, 0 , 42 , 0 , 0 , 0 , 49 , 0 , 0 , 0 , 42  , 0 , 0 , 0 , 46 , 0, 0, 0, 42 , 0 , 0 , 0 , 46 , 0 , 0 , 0 , 42 , 0, 0, 0 , 46  , 0 , 0 , 0 , 42,  0, 0, 0 , 49 , 0, 0 , 0 , 42  , 0 , 0 , 0 , 46  , 0 , 0 , 0 , 38 , 0 , 0, 0 , 46,  0, 0, 0 , 42 , 0 , 0 , 0 , 46,  0, 0, 0 , 35  , 0 , 37 , 0 , 38 , 0 , 40 , 0 , 42  , 0 , 38 , 0 , 42 , 0 , 0 , 0 , 41  , 0 , 37,  0 , 41 , 0, 0 , 0 , 40 , 0 , 36,  0 , 40 , 0 , 0 , 0 , 35 , 0 , 37 , 0, 38 , 0 , 40 , 0, 42,  0 , 38 , 0 , 42 , 0 , 47 , 0, 45,  0, 42  , 0 , 38,  0 , 42 , 0, 45  , 0 , 0, 0, 0 , 0, 0, 0 , 35 , 0 , 0, 0 , 42 , 0, 0, 0 , 35  , 0, 0 , 0 , 42 , 0 , 0 , 0 , 35 , 0 , 0 , 0 , 42 , 0 , 0, 0 , 35  , 0, 0, 0 , 42 , 0 , 0, 0 , 35 , 0 , 0, 0 , 47 , 0 , 0 , 0 , 45 , 0, 0, 0 , 43,  0, 0 , 0 , 42  , 0 , 0 , 0, 40 , 0, 0, 0 , 38  , 0 , 0, 0 , 37  , 0 };

int beatcount = 0; //to index the array tune
int string_num; //to indicate which guitar string

int flagE = 0; //flag to determine rotation (E string)
int flagA = 0; //flag to determine rotation (A string)
int flagD = 0; //flag to determine rotation (D string)
int flagG = 0; //flag to determine rotation (G string)

unsigned char loadnote; //determines which motor

volatile unsigned int timer_beat_count = 0; //sets tempo in Timer1
volatile unsigned char next_beat = 0; //interrupt flag indicating next beat

int buttonPin = 2; //Signal pin for push button or robotic conductor
int buttonState = 0;

// initialize of the Stepper library:
Stepper motor1(motorSteps, motor1Pin1, motor1Pin2);
Stepper motor2(motorSteps, motor2Pin1, motor2Pin2);
Stepper motor3(motorSteps, motor3Pin1, motor3Pin2);
Stepper motor4(motorSteps, motor4Pin1, motor4Pin2);

void setup() {
  // Initialize the Serial port:
  Serial.begin(9600);

  // set the motor speed at 200 RPM:
  motor1.setSpeed(200);
  motor2.setSpeed(200);
  motor3.setSpeed(200);
  motor4.setSpeed(200);

  pinMode(buttonPin, INPUT); //sets the pin as an input

  Start_Beat_Timer(); //Start Timer1 with selected beat

  while (next_beat == 0)  //wait for beat timer
    ;
  next_beat = 0;
}

void loop() {
  // reads buttonState after every tune is played
  while ( buttonState = digitalRead(buttonPin)) {     
    if (buttonState == HIGH) {
      // If high, code to control motors with MIDI values
      next_beat = 0;
      Start_Beat_Timer();

      for (int beatcount = 0; beatcount < tunelength; beatcount++) {
        loadnote = pgm_read_byte(&tune[beatcount]);
        Serial.print("Current note is ");
        Serial.println("Load_Note");

        Load_beat(loadnote);

        Playbeat();
      }

      while (next_beat == 0)
        ;
      next_beat = 0;
    }
    else {
      //If low, turn off motor pins
      pinMode(motor1Pin1, LOW);
      pinMode(motor1Pin2, LOW);
      pinMode(motor2Pin1, LOW);
      pinMode(motor2Pin2, LOW);
      pinMode(motor3Pin1, LOW);
      pinMode(motor3Pin2, LOW);
      pinMode(motor4Pin1, LOW);
      pinMode(motor4Pin2, LOW);
    }
  }
}

// Load beat load the motor pin to be played
void Load_beat (unsigned char Note) {
  switch (Note) {
    case 0: string_num = 5;
      break; //no note played
    case 35: string_num = 3;
      break; 
    case 36: string_num = 3;
      break; 
    case 37: string_num = 3;
      break; 
    case 38: string_num = 3;
      break; 
    case 40: string_num = 2;
      break; 
    case 41: string_num = 2;
      break; 
    case 42: string_num = 2;
      break; 
    case 43: string_num = 2;
      break;
    case 44: string_num = 2;
      break;
    case 45: string_num = 2;
      break;
    case 46: string_num = 3;
      break;
    case 47: string_num = 4;
      break; 
    case 49: string_num = 4;
      break;
    case 50: string_num = 1;
        default: break;  //do nothing
  }
}

void Playbeat(void) {

/*The string numbers and flags are used to determine which side the motor has spun so that it can spin in reverse the next time
 */

  if (string_num == 1 && flagD == 0) {
    motor1.step(100);
    flagD = 1;
  }
  else if (string_num == 2 && flagE == 0) {
    motor2.step(100);
    flagE = 1;
  }
  else if (string_num == 3 && flagA == 0) {
    motor3.step(100);
    flagA = 1;
  }
  else if (string_num == 4 && flagG == 0) {
    motor4.step(100);
    flagG = 1;
  }
  else if (string_num == 1 && flagD == 1) {
    motor1.step(-100);
    flagD = 0;
  }
  else if (string_num == 2 && flagE == 1) {
    motor2.step(-100);
    flagE = 0;
  }
  else if (string_num == 3 && flagA == 1) {
    motor3.step(-100);
    flagA = 0;
  }
  else if (string_num == 4 && flagG == 1) {
    motor4.step(-100);
    flagG = 0;
  }
  else if (string_num == 5) {
    string_num = 0 ;
  }
}

/***************************************************************************************
*    Title: Glock-o-bot code
*    Author: Mathers.D
*    Date: 2016
*    Code version: 1.0
*
***************************************************************************************/

void Start_Beat_Timer(void) {

  noInterrupts();           // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TIMSK1 |= (1 << TOIE1); // enable timer overflow interrupt
  TCNT1 = timer_beat_count;   // preload timer
  TCCR1B |= (1 << CS12);    // Sets 256 prescaler 16 uSec counts
  timer_beat_count = (65536 - (beattime / 16)); // preload timer 65536-(416666/16) = 39494
  interrupts();             // enable all interrupts

}

ISR(TIMER1_OVF_vect)        // interrupt service routine
{
  TCNT1 = timer_beat_count;   // Reload timer1 with beat count
  next_beat = 1;
}



