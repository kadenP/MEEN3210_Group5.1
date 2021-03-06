/*
 * Motor Sketch
 * 
 * This sketch reads commands from the uno controller and uses them to
 * control the actuators on the mobility platform.
 * 
 * The RX1 on Arduino Mega must be connnected to pin 2 on XBee shield
 * The TX2 on Arduino Mega must be connected to pin 3 on XBee shield
 * Make sure the small switch on the XBee Shield is in the Dline position
 *
 * Receives from the hardware serial, sends to hardware Serial2.
 * Receives from hardware Serial2, sends to hardware serial.
 * 
 * Started on 02/25/2017
 * by Kaden Plewe
*/

#include <Servo.h>
#include <DualVNH5019MotorShield.h>
#include <Encoder.h>

//Motor driver setup
DualVNH5019MotorShield md;                              //Use default pins : DualVNH5019MotorShield md(INA1,INB1,EN1DIAG1,CS1,INA2,INB2,EN1DIAG2,CS2);
//DualVNH5019MotorShield md(2,4,6,A0,7,8,12,A1);        //Use default pins
//DualVNH5019MotorShield md(42,44,46,A2,47,48,52,A3);   //Use non-default pins

//Encoder setup
Encoder encR(22, 24);
Encoder encL(28, 26);

//Encoder Variables
const long countsPerRev = 8400;          //Encoder counts per revelution of output shaft for 131:1 motor
const float wheelCircum_m = 0.314159;    //Wheel circumference
const float maxSpeed = 0.24;             //Max linear speed (m/s)

//Feedback Variables
float Kp = 0.0000001;                                        //Proportional velocity control constant
float Kd = 0.00000075;
float Ki = 0.000000005;
float desiredCntPerSecLeft, desiredCntPerSecRight;       //desired counts per second

long newPosLeft, newPosRight;                            //Tracking variables for encoder counts
long oldPosLeft = 0, oldPosRight = 0;     
long cntPerSecLeft, cntPerSecRight;                      //Variables for speed of wheels
long errorLeft = 0, errorRight = 0;                      //Velocity error signals
long prevErrorLeft = 0, prevErrorRight = 0;              //Previous error signals
long errSumLeft = 0; long errSumRight = 0;               //Error summ for integral control
unsigned long currentTime_us, oldTime_us;                //Timer variables

//Global Variables
int commandNum = 11;            //number of buckets being used in serial communication
int leftMotor = 0;              //left motor speed
int rightMotor = 0;             //right motor speed
char leftSign;                  //left motor sign
int left1;                      //first part of left motor speed signal
int left2;                      //second part of left motor speed signal
char rightSign;                 //right motor sign
int right1;                     //first part of right motor speed signal
int right2;                     //second part of right motor speed signal
int Sel;                        //select button state signal
int sgOpen;                     //Grab servo open command state
int sbUp;                       //Boom servo raise command state
int sbDown;                     //Boom servo lower command state
int sgClose;                    //Grab servo close command state
int startSignal;
int hallState = 1;              //state of hall sensor
int easyState = 1;              //state of easy button
int startTime;
int delayTime;
int blinkTime;
int lastBlink;

// *** Declare & Initialize Pins ***

const int hallPin1 = 4;
//const int hallPin2 = 32;
const int LEDhall = 42;
int LEDred = 44;
int LEDblue = 46;
int LEDteam = LEDblue;
const int LEDgreen = 45;
const int servoBoomPin = 52;
const int servoClampPin = 50;
const int easyPin = 48;
int currentLEDState = 1;



// *** Create Servo Objects ***

Servo boomServo;
Servo clampServo;


// *** Declare & Initialize Program Variables ***

int desiredPosition = 0;
int boomAngle = 0;
int clampAngle = 0;

int servoSmallInc = 1;
int servoLargeInc = 5;

void setup()
{
  md.init();
  oldTime_us = millis();
  
  // Open serial communications with computer and wait for port to open:
  Serial.begin(9600);

  // Send a message to the computer through the USB
  Serial.println("Hello Computer, I'm a Mega");

  // Open serial communications with Xbee and wait for port to open:
  Serial2.begin(9600);

  // Send a message to the Uno through the Xbee
  Serial2.print("Hello Uno, I'm a Mega!");

  //Set digital states
  pinMode(hallPin1,INPUT);
  //pinMode(hallPin2,INPUT);
  pinMode(LEDhall,OUTPUT);
  pinMode(LEDhall,OUTPUT);
  pinMode(LEDred, OUTPUT);
  pinMode(LEDblue, OUTPUT);
  pinMode(LEDgreen,OUTPUT);
  pinMode(easyPin,INPUT);
  digitalWrite(easyPin,HIGH);
  digitalWrite(LEDteam, HIGH);
  digitalWrite(LEDgreen, HIGH);

  //Configure servos
  boomServo.attach(servoBoomPin);
  boomServo.write(boomAngle);
  clampServo.attach(servoClampPin);
  clampServo.write(clampAngle);

  //Initialize signals
  leftSign = 0;          //left motor sign
  left1 = 0;             //first part of left speed signal
  left2 = 0;             //second part of left speed signal
  rightSign = 0;         //right motor sign
  right1 = 0;            //first part of right speed signal
  right2 = 0;            //second part of right speed signal
  Sel = 1;               //select button state signal
  sgOpen = 1;            //grab servo open signal
  sbUp = 1;              //boom servo up command signal
  sbDown = 1;            //boom servo down command signal
  sgClose = 1;           //grab servo close signal
}

void loop()
{
  //Test Functions
  //Default function
  //defaultTest();

  //Runs motors at full speed forward for 5 seconds and backward for 5 seconds
  //straightTest();

  //Runs motors in opposite direction at full speed, truening in a circle
  //circleTest();

  //Wireless communication test function
  //comTest();

  //Recieve serial commands from uno if all serial commands are available and active
  
  while(Serial2.available()>commandNum)
  {
    //Skip if the start signal isn't identified
    startSignal = Serial2.read();
    if(startSignal != 255)
    {
      continue;
    }

    //Recieve information
    MegaCom();
    
    //Motor Control
    driveControl(); 

    //clamp control (open/close)      
    clampControl();

    //arm control (raise/lower)       
    boomControl();

    //Check easy button in loop
    readEasy();

    //Print to serial monitor
    //serialPrint();
    
    //Indicate Team
    changeTeam();
  }
  

  //Check easy button out of loop
  readEasy();

  //Print to serial monitor
  //serialPrint();
  
}

//This function will recieve signals from the uno in the order that 
//they were sent and compile them into the appropriate commands
void MegaCom()
{
  //Otherwise, collect all expected signals in sequence with small delays in between
  int take;
  int delayTime = 0.5;                         //time for split delays
  take = Serial2.read();
  if(take != 255){leftSign = take;}          //read left motor sign
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){left1 = take;}             //read first part of left speed signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){left2 = take;}             //read second part of left speed signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){rightSign = take;}         //read right motor sign
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){right1 = take;}            //read first part of right speed signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){right2 = take;}            //read second part of right speed signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){Sel = take;}               //read select button state signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){sgOpen = take;}            //read grab servo open signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){sbUp = take;}              //read boom servo up command signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){sbDown = take;}            //read boom servo down command signal
  delay(delayTime);
  take = Serial2.read();
  if(take != 255){sgClose = take;}           //read grab servo close signal
  delay(delayTime);

  //compile left and right motor speed
  leftMotor = speedCompile(leftSign, left1, left2);
  rightMotor = speedCompile(rightSign, right1, right2);

  //Read hall effect sensor
  readHall();

  //Send signals
  MegaSend();
}

//This function will send signals to the uno
void MegaSend()
{
  //Send signal
  Serial2.write(hallState);          //State of hall effect sensor
 
}

//Use this function to test communication between maga and uno. With the coresponding function
//downloaded on the uno, the two should be able to send messages through the serial port
void comTest()
{
  if (Serial2.available())
  {
     while (Serial2.available()) 
    {
      Serial.write(Serial2.read());
        
      delay(2);
    }

  Serial.print("\n");
  }
  
   while (Serial.available()) 
  {
    Serial2.write(Serial.read());

    delay(2);
  }
  
}

//This function takes the broken up speed signal and compiles it to a format
//to send to motor shield
int speedCompile(char sign, int part1, int part2)
{
  int compSpeed;
  if(sign == 45)
  {
    compSpeed = -(part1 + part2);
  }
  else
  {
    compSpeed = part1 + part2;
  }
  
  return compSpeed;
}

//This function can be used to test the motors without the remote
void defaultTest()
{
  for (int i = 0; i <= 400; i++)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = 400; i >= -400; i--)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = -400; i <= 0; i++)
  {
    md.setM1Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M1 current: ");
      Serial.println(md.getM1CurrentMilliamps());
    }
    delay(2);
  }

  for (int i = 0; i <= 400; i++)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = 400; i >= -400; i--)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
  
  for (int i = -400; i <= 0; i++)
  {
    md.setM2Speed(i);
    stopIfFault();
    if (i%200 == 100)
    {
      Serial.print("M2 current: ");
      Serial.println(md.getM2CurrentMilliamps());
    }
    delay(2);
  }
}

//This function can be used to test the motors without the remote
void straightTest()
{
    //Run forward at full speed
    md.setM1Speed(400);
    md.setM2Speed(400);
    stopIfFault();
    Serial.print("M1 current: ");
    Serial.print(md.getM1CurrentMilliamps());
    Serial.print(", M2 current: ");
    Serial.println(md.getM2CurrentMilliamps());
    delay(5000);

    //Run backwards at full speed
    md.setM1Speed(-400);
    md.setM2Speed(-400);
    stopIfFault();
    Serial.print("M1 current: ");
    Serial.print(md.getM1CurrentMilliamps());
    Serial.print(", M2 current: ");
    Serial.println(md.getM2CurrentMilliamps());
    delay(5000);
}

//This function can be used to test the motors without the remote
void circleTest()
{
    //Spin Clockwise for 5 Seconds
    md.setM1Speed(400);
    md.setM2Speed(-400);
    stopIfFault();
    Serial.print("M1 current: ");
    Serial.print(md.getM1CurrentMilliamps());
    Serial.print(", M2 current: ");
    Serial.println(md.getM2CurrentMilliamps());
    delay(5000);

    //Spin Counter-Clockwise for 5 Seconds
    md.setM1Speed(-400);
    md.setM2Speed(400);
    stopIfFault();
    Serial.print("M1 current: ");
    Serial.print(md.getM1CurrentMilliamps());
    Serial.print(", M2 current: ");
    Serial.println(md.getM2CurrentMilliamps());
    delay(5000);
}

//This function will stop operation and indicate a failed motor
void stopIfFault()
{
  if (md.getM1Fault())
  {
    Serial.println("M1 fault");
    while(1);
  }
  if (md.getM2Fault())
  {
    Serial.println("M2 fault");
    while(1);
  }
}

//This function controls the motors
void driveControl()
{
  //Identify the encoder positions and current time
  newPosLeft = encL.read();
  newPosRight = encR.read();
  currentTime_us = micros();
//  Serial.print(newPosLeft);
//  Serial.print(" || ");
//  Serial.print(oldPosLeft);
//  Serial.print(" || ");
//  Serial.print(newPosRight);
//  Serial.print(" || ");
//  Serial.print(oldPosRight);
//  Serial.print(" || ");
//  Serial.println(currentTime_us);
  
  
  //Calculate the desired counts per second
  desiredCntPerSecLeft  = -long(map(leftMotor, -400, -maxSpeed, 400, maxSpeed) / wheelCircum_m * float(countsPerRev));
  desiredCntPerSecRight  = -long(map(rightMotor,-400, -maxSpeed, 400, maxSpeed) / wheelCircum_m * float(countsPerRev));
//  Serial.print(desiredCntPerSecLeft);
//  Serial.print(" || ");
//  Serial.println(desiredCntPerSecRight);

  //Calculate the current counts per second
  cntPerSecLeft  = (newPosLeft  - oldPosLeft)  / (1e6*(currentTime_us - oldTime_us));    
  cntPerSecRight = (newPosRight - oldPosRight) / (1e6*(currentTime_us - oldTime_us));
//  Serial.print(cntPerSecLeft);
//  Serial.print(" || ");
//  Serial.println(cntPerSecRight);

  // Determine velocity error in counts per second
  prevErrorLeft = errorLeft;
  prevErrorRight = errorRight;
  errorLeft  = desiredCntPerSecLeft - cntPerSecLeft;
  errorRight = desiredCntPerSecRight - cntPerSecRight;
//  Serial.print(errorLeft);
//  Serial.print(" || ");
//  Serial.println(errorRight);

  //Adjust motor speeds based on proportional control
  leftMotor  += int(float(errorLeft)  * Kp);  
  rightMotor += int(float(errorRight) * Kp);

  //Adjust motor speeds based on derivative control
  leftMotor  += int(float((errorLeft - prevErrorLeft)/(currentTime_us - oldTime_us))  * Kd);  
  rightMotor += int(float((errorLeft - prevErrorLeft)/(currentTime_us - oldTime_us)) * Kd);

  //Adjust motor speeds based on derivative control
  errSumLeft += errorLeft;
  errSumRight += errorRight;
  leftMotor  += int(float(errSumLeft*(currentTime_us - oldTime_us))  * Ki);  
  rightMotor += int(float(errSumRight*(currentTime_us - oldTime_us)) * Ki);
  
  //Set motor speeds
  leftMotor  = constrain(leftMotor, -400,400);
  rightMotor = constrain(rightMotor,-400,400);
  if(leftMotor < 5 && leftMotor > -5){md.setM1Brake(400);}
  else
  {
    md.setM1Speed(leftMotor);
    stopIfFault();
  }
  if(rightMotor < 5 && rightMotor > -5){md.setM2Brake(400);}
  else
  {
    md.setM2Speed(rightMotor);
    stopIfFault();
  }

  //Set variables for next cycle
  oldTime_us  = currentTime_us;
  oldPosLeft  = newPosLeft;
  oldPosRight = newPosRight;
}

//This function controls the manipulator arm
void boomControl() 
{
  if (sbUp == 0 && boomAngle > 0)
  {
    boomAngle = boomAngle - servoSmallInc;
    boomServo.write(boomAngle);
    //Serial.println("Raising");

  }
  if (sbDown == 0 && boomAngle < 105)
  {
    boomAngle = boomAngle + servoSmallInc;
    boomServo.write(boomAngle);
    //Serial.println("Lowering");
  } 
}

//This function controls the clamp
void clampControl()
{
  if (sgOpen == 0 && clampAngle < 180)
  {
    clampAngle = clampAngle + servoLargeInc;
    clampServo.write(clampAngle);
    //Serial.println("Opening");
  }
  if (sgClose == 0 && clampAngle > 0)
  {
    clampAngle = clampAngle - servoLargeInc;
    clampServo.write(clampAngle);
    //Serial.println("Closing");

  }
}

//This function reads the hall effect sensor
void readHall()
{
  if(analogRead(hallPin1) > 100){hallState = 0;}
  else{hallState = 1;}
}

void readEasy()
{
  easyState = digitalRead(easyPin);
  if(easyState == 0)
  {
    Serial.print("**********************************************************************");
    //Stop Motors
    md.setM1Speed(0);
    md.setM2Speed(0);
    startTime = millis();
    delayTime = millis() - startTime;
    lastBlink = 0;
    delay(2000);
    while(delayTime < 30000)
    {
      //Blink LEDs
      if(delayTime - lastBlink > 1000)
      {
        if(currentLEDState == 0)
        {
          //Turn on
          digitalWrite(LEDteam, HIGH);
          digitalWrite(LEDgreen, HIGH);
          currentLEDState = 1;
        }
        else
        {
          //Turn off
          digitalWrite(LEDteam, LOW);
          digitalWrite(LEDgreen, LOW);
          currentLEDState = 0;
        }
        lastBlink = delayTime;
      }
      
      easyState = digitalRead(easyPin);
      Serial.print(easyState);
      Serial.print(" || Delayed: ");
      Serial.print(delayTime);
      Serial.print(" || Last Blink: ");
      Serial.println(lastBlink);

      //Update delay time
      delayTime = millis() - startTime;

      //End timer if easy button is pressed again 
      if(easyState == 0){easyState = 1; delay(1000); delayTime = 30000;}
      //if(easyState == 0){easyState = 1; delay(1000); break;}
    }
  }
  if(currentLEDState == 0)
  {
    //Turn on
    digitalWrite(LEDteam, HIGH);
    digitalWrite(LEDgreen, HIGH);
    currentLEDState = 1;
  }
}

void changeTeam()
{
  if(millis() < 15000)
  {
    digitalWrite(LEDteam, LOW);
    if(sbUp == 0 && sbDown == 0)
    {
      LEDteam = LEDred;
    }
    if(sgOpen == 0 && sgClose == 0)
    {
      LEDteam = LEDblue;
    }
    digitalWrite(LEDteam, HIGH);
  }
}

void serialPrint()
{
  //Print Signals
  Serial.print(analogRead(hallPin1));
  Serial.print(" | ");    
  Serial.print(Sel);
  //Serial.print(" | ");
  //Serial.print(sgOpen);
  //Serial.print(" | ");
  //Serial.print(sbUp);
  //Serial.print(" | ");
  //Serial.print(sbDown);
  //Serial.print(" | ");
  //Serial.print(sgClose);
  //Serial.print(" | ");
  //Serial.print("left motor: ");
  //Serial.print(leftMotor);
  //Serial.print(" right motor: ");
  //Serial.print(rightMotor);
  //Serial.print(easyState);
  Serial.println(); 
}
