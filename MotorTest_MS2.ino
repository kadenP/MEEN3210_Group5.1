#include "DualVNH5019MotorShield.h"
DualVNH5019MotorShield md;//Use default pins : DualVNH5019MotorShield md(INA1,INB1,EN1DIAG1,CS1,INA2,INB2,EN1DIAG2,CS2);
//DualVNH5019MotorShield md(2,4,6,A0,7,8,12,A1);// Use default pins
//DualVNH5019MotorShield md(42,44,46,A2,47,48,52,A3);//Use non-default pins

//#include "DualVNH5019MotorShieldMod.h"

//DualVNH5019MotorShieldMod md;//Use default pins : DualVNH5019MotorShield md(INA1,INB1,EN1DIAG1,CS1,INA2,INB2,EN1DIAG2,CS2);
//DualVNH5019MotorShieldMod md(2,4,6,A0,7,8,12,A1);// Use default pins
//DualVNH5019MotorShieldMod md(42,44,46,A2,47,48,52,A3);//Use non-default pins
//DualVNH5019MotorShieldMod md(42,44,46,A2,47,48,52,A3,11,12);//Use non-default pins including non-default PWM

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

void setup()
{
  Serial.begin(115200);
  Serial.println("Dual VNH5019 Motor Shield");
  md.init();
}

void loop()
{
  //Select one function to run on robot

  //Default function (definitely works) if not working, check pin declarations at top
  defaultTest();

  //Runs motors at full speed forward for 5 seconds and backward for 5 seconds
  //straightTest();

  //Runs motors in opposite direction at full speed, truening in a circle
  //circleTest();
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
