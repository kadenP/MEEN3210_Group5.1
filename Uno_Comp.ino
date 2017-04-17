/* 
 *  Joystick Sketch
 *  
 *  This scketch reads the inputs from the Sparkfun Joystick
 *  Shield and sets up the outputs to control the actuators 
 *  used on the robot.
 *  
 *  http://www.sparkfun.com/commerce/product_info.php?products_id=9490
 *  
 *  Started on 2/12/2017
 *  by Kaden Plewe
 */

#include <SoftwareSerial.h>
SoftwareSerial mySerial(2,7);   //Set up XBee on pins 2 and 7
 

//Joystick Button (Digital) Pins
const int Bu0 = 3;    //Right Button
const int Bu1 = 4;    //Top Button
const int Bu2 = 5;    //Bottom Button
const int Bu3 = 6;    //Left Button
const int Sel = 2;    //Select Button (on Joystick)

//Joystick Pot (Analog) Pins
const int xPot = 0;  //X-Axis Potentiometer
const int yPot = 1;  //Y-Axis Potentiometer

//Global Variables
int leftMotor;
int rightMotor;
char leftSign;
char rightSign;
int left1;
int left2;
int right1;
int right2;

// LED pins
int led = 13;
int led_G = 12;
int led_Y = 11;
int led_R = 10;

//Other Variables
int commandNum = 1;            //number of expected signals from Mega (-1)
int startSignal;
int yellowState;
int startTime;
int hallState = 1;


// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(led, OUTPUT);
  pinMode(led_G, OUTPUT);
  pinMode(led_Y, OUTPUT);
  pinMode(led_R, OUTPUT);
  
  pinMode(Sel, INPUT);      //Set Joystick Select as input
  digitalWrite(Sel, HIGH);  //Initialize pull-up resistor on Select 

  pinMode(Bu0, INPUT);      //Set Right Button as input
  digitalWrite(Bu0, HIGH);  //Initialize pull-up resistor on Right Button

  pinMode(Bu1, INPUT);      //Set Top Button as input
  digitalWrite(Bu1, HIGH);  //Initialize pull-up resistor on Top Button

  pinMode(Bu2, INPUT);       //Set Bottom Button as input
  digitalWrite(Bu2, HIGH);  //Initialize pull-up resistor on Bottom Button

  pinMode(Bu3, INPUT);      //Set Left Button as input
  digitalWrite(Bu3, HIGH);  //Initialize pull-up resistor on Left Button
  
  // Open serial communications with computer and wait for port to open:
  Serial.begin(9600);

  // Send a message to the computer through the USB
  Serial.println("Hello Computer, I'm a Uno");

  // Open serial communications with Xbee and wait for port to open:
  mySerial.begin(9600);

  // Send a message to the Mega through the Xbee
  mySerial.print("Hello Mega, I'm a Uno!");

  //Turn on Green LED to indicate power
  digitalWrite(led_G, HIGH);
}

//Run loop continuously
void loop() 
{

  //Wireless communication test function
  //comTest();

  //Always send signals
  unoCom();
  

}

/***********************************************************************************
This function takes raw values from the joystick pots and converts them into
left/right mootor signals between -400 and 400 to be send wirelessly to the Mega
***********************************************************************************/
int motorInput(int xRaw, int yRaw)
{
  //Define the throttle value from y position
  int throttle = map(yRaw, 0, 1023, -400, 400);
  
  //Define the direction value from x position
  int direc = map(xRaw, 0, 1023, -300, 300);
  
  //Motor Inputs
  if((xRaw < 510 && xRaw > 490) && (yRaw < 510 && yRaw > 490))
  {
    leftMotor = 0;
    rightMotor = 0;
  }
  else
  {
    //negative sign because motors were mounted opposite
    leftMotor = -constrain(throttle + direc, -400, 400);
    rightMotor = -constrain(throttle - direc, -400, 400);
  }
}

/***********************************************************************************
This function sends the data from each controller button/joystick to the Mega
***********************************************************************************/
void unoCom()
{
  //Read x and y positions and map
  int xRaw = analogRead(xPot);
  int yRaw = analogRead(yPot);
  int xMap = map(xRaw, 0, 1023, -10, 11);
  int yMap = map(yRaw, 0, 1023, -10, 11);

  //Get values to send to Mega
  motorInput(xRaw, yRaw);
      
  //Break up the motor signals to be between 0 and 254
  if(leftMotor < 0)
  {
    leftSign = 45;    //negative signal (ASCII 45)
  }
  else
  {
    leftSign = 43;   //positive signal (ASCII 43)
  }

  if(rightMotor < 0)
  {
    rightSign = 45;    //negative signal (ASCII 45)
  }
  else
  {
    rightSign = 43;    //positive signal (ASCII 43)
  }
  
  left1 = constrain(abs(leftMotor), 0, 254);
  left2 = abs(leftMotor) - left1;
  right1 = constrain(abs(rightMotor), 0, 254);
  right2 = abs(rightMotor) - right1;

  //Recieve Information
  unoRecieve();
  
  //Send signals
  mySerial.write(255);                //signal to indicate starting bucket
  //Serial.print(255);
  //Serial.print(" | ");
  mySerial.write(leftSign);           //sign on left motor signal
  //Serial.print(leftSign);
  //Serial.print(" | ");
  mySerial.write(left1);              //first part of left motor signal
  //Serial.print(left1);
  //Serial.print(" | ");
  mySerial.write(left2);              //second part of left motor signal
  //Serial.print(left2);
  //Serial.print(" | ");
  mySerial.write(rightSign);          //sign on right motor signal
  //Serial.print(rightSign);
  //Serial.print(" | ");
  mySerial.write(right1);             //first part of right motor signal
  //Serial.print(right1);
  //Serial.print(" | ");
  mySerial.write(right2);             //second part of right motor signal
  //Serial.print(right2);
  //Serial.print(" | ");
  mySerial.write(digitalRead(Sel));   //select button on joystick (0 or 1)
  //Serial.print(digitalRead(Sel));
  //Serial.print(" | ");
  mySerial.write(digitalRead(Bu0));   //right button (0 or 1)
  //Serial.print(digitalRead(Bu0));
  //Serial.print(" | ");
  mySerial.write(digitalRead(Bu1));   //top button (0 or 1)
  //Serial.print(digitalRead(Bu1));
  //Serial.print(" | ");
  mySerial.write(digitalRead(Bu2));   //bottom button (0 or 1)
  //Serial.print(digitalRead(Bu2));
  //Serial.print(" | ");
  mySerial.write(digitalRead(Bu3));   //left button (0 or 1)
  //Serial.println(digitalRead(Bu3));
  
}

/***********************************************************************************
This function will read the data from the Mega in the order in which they were sent
***********************************************************************************/
void unoRecieve()
{
  Serial.print(mySerial.available());
  Serial.print(" || ");
  if(mySerial.available())
  {
    digitalWrite(led_Y, HIGH);                               //Indicate Connection
     
    //Recieve serial commands from uno if all serial commands are available and active
    int take = mySerial.read();
    if(take < 2){hallState = take;}                          //Hall sensor reading
    if(hallState == 0){digitalWrite(led_R, HIGH);}           //Indicate magnet
    else{digitalWrite(led_R, LOW);}                          //Indicate no magnet
  }
   
  else{digitalWrite(led_Y, LOW);}                            //Indicate no connection
  Serial.println(hallState);
}

/***********************************************************************************
Use this function to test communication between maga and uno. With the coresponding 
function downloaded on the mega, the two should be able to send messages through 
the serial port
***********************************************************************************/
void comTest()
{
 if(mySerial.available())
 {
  while(mySerial.available()) 
   {
     Serial.write(mySerial.read());   
     delay(2);  
   }
  Serial.print("\n");  
 }
 
 while (Serial.available()) 
 {
     mySerial.write(Serial.read());
     delay(2);
 }
}
