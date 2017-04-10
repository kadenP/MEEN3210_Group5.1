//Include Libraries that will be used
#include <LiquidCrystal.h>                        //LCD Library
#include <DualVNH5019MotorShield.h>               //Motor Library

LiquidCrystal lcd(53,51,49,47,45,43);
DualVNH5019MotorShield md;

//Declare variables and constants
const int switchHigh = 42;                                // Start switch 5V source
const int switchSignal = 40;                              // Start switch signal
const int switchLow = 44;                                 // Start switch ground connection
int motorSpeed_Fwd[5] = {10, 100, 200, 300, 400};         // Forward motor speeds at 0, 25, 50, 75, and 100% PWM duty cycles
int motorSpeed_Rev[5] = {-10, -100, -200, -300, -400};    // Reverse motor speeds at 0, 25, 50, 75, and 100% PWM duty cycles
int motorCoast = 0;                                       // Value to allow motors to coast

//Initializes pins and devices
void setup() 
{
  //User Displays
  lcd.begin(16, 2);          //Initialize lcd screen with 16 columns and 2 rows
  Serial.begin(9600);        //Initialize serial communication

  //Start Switch
  pinMode(switchHigh, OUTPUT);
  digitalWrite(switchHigh, HIGH);
  pinMode(switchLow, OUTPUT);
  digitalWrite(switchLow, LOW);
  pinMode(switchSignal, INPUT);
  
}

void loop() 
{
  //Set LCD Display
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press Start");
  
  //Hold untill start button is pressed
  while(!digitalRead(switchSignal))
  {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Running...");

    //Run motor control function
    motorControl(motorSpeed_Fwd, motorSpeed_Rev);
  }
}
