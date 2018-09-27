/* Includes */
#include <Servo.h> // For servo control
#include "BasicStepperDriver.h" // For stepper control
#include <SoftwareSerial.h> // for serial 

/* Setting up Serial Object */
static char data; // To hold received characters
const byte rxPin = 1;
const byte txPin = 0;
SoftwareSerial mySerial = SoftwareSerial(rxPin, txPin);

/* Servos */
Servo servoSlide;
Servo servoHook;

/* Stepper */
#define MOTOR_STEPS 400 // # steps/rev 
#define RPM 220 // 120 RPM motor speed
#define MICROSTEPS 4 // must match external motor setting (can be modified)
const int rowStep = 10870; // degress to move up/down an entire row
const int hookStep = 5400; // degress to move up/down to attach/detach from hooks
#define STEP 3
#define DIR 4
// Uncomment line to use enable/disable functionality
// #define ENABLE 
// 2-wire basic config, microstepping is hardwired on the driver
BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP);
//Uncomment line to use enable/disable functionality
//BasicStepperDriver stepper(MOTOR_STEPS, DIR, STEP, ENABLE);

/* Instructions */
const char loadR = '1'; // load round pill
const char loadF = '2'; // load flat pill
const char loadL = '3'; // load long pill
const char slideColumn1 = '4'; // move servo slide to column 1
const char slideColumn2 = '5'; // move servo slide to column 2
const char slideColumn3 = '6'; // move servo slide to column 3
const char slideColumn4 = '7'; // move servo slide to column 4
const char slideResR = '8'; // servo slide leads to reservoir for round pills
const char slideResF = '9'; // servo slide leads to reservoir for flat pills
const char slideResL = 'A'; // servo slide leads to reservoir for long pills
const char hookDetach = 'B'; // retract servo hooks
const char hookAttach = 'C'; // protract servo hooks
const char raiseRow = 'D'; // increase height by a row
const char lowerRow1 = 'E'; // lower height by  one row
const char lowerRow2 = 'Q'; // lower height by two rows
const char lowerRow3 = 'R'; // lower height by three rows
const char smallRaise = 'F'; // small increment height raise
const char smallLower = 'G'; // small increment height decrease
const char manualRaise = 'H'; // manual incremental raise of the cabinet
const char manualLower = 'I'; // manual incremental lower of the cabinet
const char stepperSleepOff = 'J'; // turns off sleep mode for stepper motors
const char stepperSleepOn = 'K'; // turns on sleep mode for stepper motors
const char IRsensor1 = 'L'; // checks sensor 1 for marker
const char IRsensor2 = 'M'; // checks sensor 2 for marker
const char IRsensor3 = 'N'; // checks sensor 3 for marker
const char IRsensor4 = 'O'; // checks sensor 4 for marker
const char pillDrop = 'P'; // checks for pill drop within 1 second

/* Setup */
void setup() {
  /* PIC Communication */
  /* Configure pin modes for tx and rx. */
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);
  /* Open software serial port with baud rate = 9600. */
  mySerial.begin(9600);

  // Initializing all digital pins
  short int i = 0;
  for (i = 0; i < 10; i++){
    pinMode(i + 2, OUTPUT);
    digitalWrite(i + 2, LOW);
  }

  // Initializing Ananlog Pins
  pinMode(A0, INPUT); // IR sensor 1
  pinMode(A1, INPUT); // IR sensor 2
  pinMode(A2, INPUT); // IR sensor 3
  pinMode(A3, INPUT); // IR sensor 4
  pinMode(A4, INPUT); // Breakbeam
  
  /* Servo Pin Setup */
  servoSlide.attach(7);
  servoHook.attach(8);
  /* Servo Position Initialization */
  servoSlide.write(101);
  servoHook.write(107);
  
  /* Stepper */
  stepper.begin(RPM, MICROSTEPS);
}

/* Loop, polling for PIC message */
void loop() { 
  int j; 
  long int q = 0;
  short int trig = 0;
  
  /* Wait to receive the message from the main PIC. */
  while(mySerial.available() < 1){ continue; }

  /* Write received byte to the LCD using ASCII encoding.
   * The received byte can also be seen on your PC under
   * Tools -> Serial Monitor. */
  data = mySerial.read();
  
  /* Switch statement encompassing all possible motor commands */
  switch(data){
    case loadR:
      digitalWrite(9, HIGH);
      delay(210);
      digitalWrite(9, LOW);
      break;
    case loadF:
      digitalWrite(10, HIGH);
      delay(200);
      digitalWrite(10, LOW);
      break;
    case loadL:
      digitalWrite(11, HIGH);
      delay(170);
      digitalWrite(11, LOW);
      break;
    case slideColumn1:
      servoSlide.write(101); 
      break;
    case slideColumn2:
      servoSlide.write(80); 
      break;
    case slideColumn3:
      servoSlide.write(60); 
      break;
    case slideColumn4:
      servoSlide.write(45); 
      break;
    case hookDetach:
      for (j = 161; j >= 107; j--){
        servoHook.write(j);
        delay(15);}
      break;
    case hookAttach:
      for (j = 107; j <= 161; j++){
        servoHook.write(j);
        delay(15);}
      break;      
    case raiseRow:
      digitalWrite(2, HIGH);
      stepper.rotate(-rowStep);
      delay(1);
      digitalWrite(2, LOW);
      break;
    case lowerRow1:
      digitalWrite(2, HIGH);
      stepper.rotate(rowStep);
      delay(1);
      digitalWrite(2, LOW);
      break;
    case lowerRow2:
      digitalWrite(2, HIGH);
      stepper.rotate(2*rowStep);
      delay(1);
      digitalWrite(2, LOW);
      break;
    case lowerRow3:
      digitalWrite(2, HIGH);
      stepper.rotate(3*rowStep);
      delay(1);
      digitalWrite(2, LOW);
      break;
    case smallRaise:
      digitalWrite(2, HIGH);
      stepper.rotate(-hookStep);
      delay(1);
      digitalWrite(2, LOW);
      break;
    case smallLower:
      digitalWrite(2, HIGH);
      stepper.rotate(hookStep);
      delay(1);
      digitalWrite(2, LOW);
      break;
    case slideResR:
      servoSlide.write(159);
      break;
    case slideResF:
      servoSlide.write(139);
      break;
    case slideResL:
      servoSlide.write(122);
      break;
    case manualRaise:
      stepper.rotate(-20);
      break;
    case manualLower:
      stepper.rotate(20);
      break;
    case stepperSleepOff:
      digitalWrite(2, HIGH);
      break; 
    case stepperSleepOn:
      digitalWrite(2, LOW);
      break;
    case IRsensor1:
      if (analogRead(A3) > 70){ mySerial.write(1);} 
      else{ mySerial.write(2);}
      break;
    case IRsensor2:
      if (analogRead(A2) > 500){ mySerial.write(1);} 
      else{ mySerial.write(2);}
      break;
    case IRsensor3:
      if (analogRead(A1) > 500){ mySerial.write(1);} 
      else{ mySerial.write(2);}
      break;
    case IRsensor4:
      if (analogRead(A0) > 500){ mySerial.write(1);} 
      else{ mySerial.write(2);}
      break;
    case pillDrop:
      while((q < 6000)&&(trig < 1)){
        if (analogRead(A4) == 0){ trig++;}
        else{ trig = 0;}
        q++;}
      if (q < 6000){ mySerial.write(1);}
      else{ mySerial.write(2);}
      break;  
  }
  data = '0';
}



