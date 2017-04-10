/****************************************************************
Author Name: Trevor Teerlink    
Date: 2/21/17
Sketch Name: MS4 servo test
Sketch Description:  moves servos for manipulator arm.

Button Usage: Up/Down    -  raise lower arm
              Left/Right -  open close grabber
              Select     -  

Pin Usage:    Pin type/number     Hardware 
              ----------------    ----------------                   
              
*******************************************************************/
  #include<Servo.h>


// *** Declare & Initialize Pins ***

const int hallPin = A1;
const int LEDhall = 1;
const int LEDred = 12;
const int LEDblue = 13;
const int servoBoomPin = 5;
const int servoClampPin = 6;
const int easyPin = 11;

// *** Create Servo Objects ***

Servo boomServo;
Servo clampServo;

// *** Declare & Initialize Program Variables ***

int desiredPosition = 0;
int boomAngle = 90;
int clampAngle = 0;

int servoSmallIncrement = 1;
int servoLargeIncrement = 5;

int hallSensorReading = 0;
char command;


void setup() {
  // put your setup code here, to run once:

pinMode(hallPin,INPUT);
pinMode(LEDhall,OUTPUT);
pinMode(LEDhall,OUTPUT);
pinMode(LEDred,OUTPUT);
pinMode(LEDblue,OUTPUT);

boomServo.attach(servoBoomPin);
clampServo.attach(servoClampPin);


  // *** Initialize Serial Communication ***
     Serial.begin(9600);


  // *** Move Hardware to Desired Initial Positions ***

boomServo.write(boomAngle);
clampServo.write(clampAngle);


}


void loop() {
  // put your main code here, to run repeatedly:


  if(Serial.available() != 0){
    command = Serial.read();
  }

if (command == 'u'){
  boomAngle = boomAngle + 2;
  boomServo.write(boomAngle);
}

if (command == 'd'){
  boomAngle = boomAngle - 2;
  boomServo.write(boomAngle);
}

if (command = 'o'){
  clampAngle = 0;
  clampServo.write(clampAngle);
}
if (command = 'c'){
  clampAngle = 60;
  clampServo.write(clampAngle);
}

}
