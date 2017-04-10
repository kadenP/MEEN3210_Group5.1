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
 * Receives from the hardware serial, sends to hardware serial1.
 * Receives from hardware serial1, sends to hardware serial.
 * 
 * Started on 02/25/2017
 * by Kaden Plewe
*/

#include "DualVNH5019MotorShield.h"
DualVNH5019MotorShield md;                              //Use default pins : DualVNH5019MotorShield md(INA1,INB1,EN1DIAG1,CS1,INA2,INB2,EN1DIAG2,CS2);
//DualVNH5019MotorShield md(2,4,6,A0,7,8,12,A1);        //Use default pins
//DualVNH5019MotorShield md(42,44,46,A2,47,48,52,A3);   //Use non-default pins

//Constants
const int SB = 5;        //Digital pin 5 for boom servo
const int SG = 6;        //Digital pin 6 for grab servo

//Global Variables
int commandNum = 11;            //number of buckets being used in serial communication
int leftMotor;                  //left motor speed
int rightMotor;                 //right motor speed
char leftSign;                  //left motor sign
int left1;                      //first part of left motor speed signal
int left2;                      //second part of left motor speed signal
char rightSign;                 //right motor sign
int right1;                     //first part of right motor speed signal
int right2;                     //second part of right motor speed signal
int Sel;                        //select button state signal
int sgOpen;                   //Grab servo open command state
int sbUp;                       //Boom servo raise command state
int sbDown;                     //Boom servo lower command state
int sgClose;                  //Grab servo close command state
int startSignal;

void setup()
{
  md.init();
  
  // Open serial communications with computer and wait for port to open:
  Serial.begin(9600);

  // Send a message to the computer through the USB
  Serial.println("Hello Computer, I'm a Mega");

  // Open serial communications with Xbee and wait for port to open:
  Serial1.begin(9600);

  // Send a message to the Uno through the Xbee
  Serial1.print("Hello Uno, I'm a Mega!");
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

  //Always send signals
  //MegaSend();

  //Recieve serial commands from uno if all serial commands are available and active
  while(Serial1.available()>commandNum)
  {
    //Skip if the start signal isn't identified
    startSignal = Serial1.read();
    if(startSignal != 255)
    {
      continue;
    }

    //Send and recieve information
    MegaRecieve();
    
  }
  
}

//This function will recieve signals from the uno in the order that 
//they were sent and compile them into the appropriate commands
void MegaRecieve()
{
  //Serial.print("Collecting Data");
  //Otherwise, collect all expected signals in sequence with small delays in between
  int delayTime = 5;                  //time for split delays
  leftSign = Serial1.read();          //read left motor sign
  delay(delayTime);
  left1 = Serial1.read();             //read first part of left speed signal
  delay(delayTime);
  left2 = Serial1.read();             //read second part of left speed signal
  delay(delayTime);
  rightSign = Serial1.read();         //read right motor sign
  delay(delayTime);
  right1 = Serial1.read();            //read first part of right speed signal
  delay(delayTime);
  right2 = Serial1.read();            //read second part of right speed signal
  delay(delayTime);
  Sel = Serial1.read();               //read select button state signal
  Serial.print(Sel);
  Serial.print(" | ");
  delay(delayTime);
  sgOpen = Serial1.read();            //read grab servo open signal
  Serial.print(sgOpen);
  Serial.print(" | ");
  delay(delayTime);
  sbUp = Serial1.read();              //read boom servo up command signal
  Serial.print(sbUp);\
  Serial.print(" | ");
  delay(delayTime);
  sbDown = Serial1.read();            //read boom servo down command signal
  Serial.print(sbDown);
  Serial.print(" | ");
  delay(delayTime);
  sgClose = Serial1.read();           //read grab servo close signal
  Serial.print(sgClose);
  Serial.print(" | ");
  delay(delayTime);

  //compile left and right motor speed
  leftMotor = speedCompile(leftSign, left1, left2);
  Serial.print("left motor: ");
  Serial.print(leftMotor);
  rightMotor = speedCompile(rightSign, right1, right2);
  Serial.print(" right motor: ");
  Serial.print(rightMotor);
  Serial.println();

  //Set motor speeds
  md.setM1Speed(leftMotor);
  stopIfFault();
  md.setM2Speed(rightMotor);
  stopIfFault();
  
  //Avoid problematic button combinations
  if(sbUp == 0 && sbDown == 0)
  {
    sbUp = 1;
    sbDown = 1;
  }
  if(sgOpen == 0 && sgClose == 0)
  {
    sgOpen = 1;
    sgClose = 1;
  }
}

//This function will send signals to the uno
void MegaSend()
{
  //Send signals
  Serial1.write(255);                //signal to indicate starting bucket
  Serial1.write(1);                  //Indicate connection for yellow LED on uno
 
}

//Use this function to test communication between maga and uno. With the coresponding function
//downloaded on the uno, the two should be able to send messages through the serial port
void comTest()
{
  if (Serial1.available())
  {
     while (Serial1.available()) 
    {
      Serial.write(Serial1.read());
        
      delay(2);
    }

  Serial.print("\n");
  }
  
   while (Serial.available()) 
  {
    Serial1.write(Serial.read());

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
