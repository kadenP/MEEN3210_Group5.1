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

//Joystick Button (Digital) Pins
const int Bu0 = 3;    //Right Button
const int Bu1 = 4;    //Top Button
const int Bu2 = 5;    //Bottom Button
const int Bu3 = 6;    //Left Button
const int Sel = 2;   //Select Button (on Joystick)

//Joystick Pot (Analog) Pins
const int xPot = 0;  //X-Axis Potentiometer
const int yPot = 1;  //Y-Axis Potentiometer

//Global Variables
int leftMotor;
int rightMotor;

void setup() 
{
  pinMode(Sel, INPUT);      //Set Joystick Select as input
  digitalWrite(Sel, HIGH);  //Initialize pull-up resistor on Select 

  pinMode(Bu0, INPUT);      //Set Right Button as input
  digitalWrite(Bu0, HIGH);  //Initialize pull-up resistor on Right Button

  pinMode(Bu1, INPUT);      //Set Top Button as input
  digitalWrite(Bu1, HIGH);  //Initialize pull-up resistor on Top Butto

  pinMode(Bu2, INPUT);       //Set Bottom Button as input
  digitalWrite(Bu2, HIGH);  //Initialize pull-up resistor on Bottom Button

  pinMode(Bu3, INPUT);      //Set Left Button as input
  digitalWrite(Bu3, HIGH);  //Initialize pull-up resistor on Left Button
  
  Serial.begin(9600);

}

void loop() 
{
  //Read x and y positions and map
  int xRaw = analogRead(xPot);
  int yRaw = analogRead(yPot);
  int xMap = map(xRaw, 0, 1023, -10, 11);
  int yMap = map(yRaw, 0, 1023, -10, 11);

  motorInput(xRaw, yRaw);
  
  //Read and print button and pot states on serial monitor
  Serial.print("Joystick Coordinates: ");
  Serial.print(xMap);                     //x-position of joystick
  Serial.print(", ");
  Serial.print(yMap);                     //y-position of joystick
  Serial.print("   Motor Inputs (L, R): ");
  Serial.print(leftMotor);                //Left motor input
  Serial.print(", ");
  Serial.print(rightMotor);               //Right motor input
  Serial.print("   Button Status: ");
  Serial.print(digitalRead(Sel));         //Select Button
  Serial.print(digitalRead(Bu0));         //Right Button
  Serial.print(digitalRead(Bu1));         //Top Button
  Serial.print(digitalRead(Bu2));         //Bottom Button
  Serial.println(digitalRead(Bu3));       //Left Button
  
  delay(10);
}










