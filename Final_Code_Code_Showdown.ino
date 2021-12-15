/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.

 modified 8 Nov 2013
 by Scott Fitzgerald
 http://www.arduino.cc/en/Tutorial/Sweep
*/

#include <Servo.h>

const int LED = 13; //LED at pin 13
const int magnet = 8; //magnet at pin 8
float xposition;
float yposition;
int sortshape;
const float sarm = 15;
const float larm = 18;
float bMotor = 0.0;
float sMotor = 0.0;
float aMotor = 0.0;
float baseAngle = 0.0;
float shoulderAngle = 0.0;
float armAngle = 0.0;


const byte buffSize = 40;
unsigned int inputBuffer[buffSize];
const char startMarker = '<';
const char endMarker = '>';
byte bytesRecvd = 0;
boolean readInProgress = false;
boolean newDataFromPC = false;
byte coordinates[3];

Servo base;
Servo shoulder;
Servo arm;
Servo mag;

void setup()
{ // set up arduinos and start serial monitoring
  pinMode(magnet, OUTPUT);
  pinMode(LED, OUTPUT);
  base.attach(9);
  shoulder.attach(10);
  arm.attach(11);
  mag.attach(8);
  
  Serial.begin(9600);
  Serial.println("begin");
}

void loop()
{
  //get coordinates from PC
  getDataFromPC();

if(newDataFromPC)
  {
    sortshape = coordinates[0];
    xposition = coordinates[1];
    yposition = coordinates[2];

    digitalWrite(LED, HIGH);
    //detectShapes(); //triangle or sqaure?
   // magnetON(); //turn on magnet
    calcAngles();
    motorStartingPosition(); //motor goes to pick up shape
    //sortingShapes(); //where does it go?
    //magnetOFF(); //turn off magnet

    sendEnableCmd();
    sendCoordinatesToPC(); //send coordinates of shape
    newDataFromPC = false;
    digitalWrite(LED, LOW);
  }
}

void magnetON()
{
  delay(1000);
  mag.write(180);
  digitalWrite(magnet, HIGH);
  delay(1000);
  mag.write(0);
}

void magnetOFF()
{
  digitalWrite(magnet,LOW);
  delay(2000);
}

void calcAngles() //calculate motor angles
{
  //triangle math for base swivel servo
  float width = 0.0;
  float height = 0.0;
  
  //directly in front of robot
  if (xposition == 11.0)
  {
    baseAngle = 90.0;
    //triangle math for shoulder and arm servos
  double const robotHeight = 7.0;
  double hyp = sqrt(sq(robotHeight)+sq(height));

  //law of cosines
  armAngle = acos((sq(larm)+sq(sarm)-sq(hyp))/(2.0*larm*sarm));
  shoulderAngle = acos((sq(hyp)+sq(larm)-sq(sarm))/(2.0*hyp*larm));

  //convert to degrees
  armAngle = (armAngle*180.0)/3.14159;
  armAngle = armAngle + 30.0;
  shoulderAngle = (shoulderAngle*180.0)/3.14159;
  shoulderAngle = (90.0 + shoulderAngle);
  }
  
  else if (xposition > 11.0)
  {
    width = (xposition - 11.0);
    height = (11.0 + yposition);
    baseAngle = (height/width);
    baseAngle = atan(baseAngle);
  Serial.print("<C");
  Serial.print("height :");
  Serial.print(height);
  Serial.print("width :");
  Serial.print(width);
  Serial.println(">");

    //triangle math for shoulder and arm servos
  double const robotHeight = 7.0;
  double hyp = sqrt(sq(robotHeight)+sq(height));

  //law of cosines
  armAngle = acos((sq(larm)+sq(sarm)-sq(hyp))/(2.0*larm*sarm));
  shoulderAngle = acos((sq(hyp)+sq(larm)-sq(sarm))/(2.0*hyp*larm));

  //convert to degrees
  baseAngle = (baseAngle*180.0)/3.14159;
  armAngle = (armAngle*180.0)/3.14159;
  armAngle = armAngle + 30.0;
  shoulderAngle = (shoulderAngle*180.0)/3.14159;
  shoulderAngle = (90.0 + shoulderAngle);
  }

  else if (xposition < 11.0)
  {
    width = abs(xposition - 11.0);
    height = (11.0 + yposition);
    baseAngle = (height/width);
    baseAngle = atan(baseAngle);

    //triangle math for shoulder and arm servos
  double const robotHeight = 7.0;
  double hyp = sqrt(sq(robotHeight)+sq(height));

  //law of cosines
  armAngle = acos((sq(larm)+sq(sarm)-sq(hyp))/(2.0*larm*sarm));
  shoulderAngle = acos((sq(hyp)+sq(larm)-sq(sarm))/(2.0*hyp*larm));

  //convert to degrees
  baseAngle = (baseAngle*180.0)/(3.14159);
  baseAngle = (180-baseAngle);
  armAngle = (armAngle*180.0)/(3.14159);
  armAngle = armAngle + 30.0;
  shoulderAngle = (shoulderAngle*180.0)/(3.14159);
  shoulderAngle = (90.0 + shoulderAngle);
  }
  
  //print out the angles
  Serial.print("<D");
  Serial.print("base angle:");
  Serial.print(baseAngle);

  Serial.print(", arm angle:");
  Serial.print(armAngle);

  Serial.print(", shoulder angle:");
  Serial.print(shoulderAngle);
  Serial.println(">");
}

//void detectShapes() //triangle or sqaure?
//{
  
//}

void sortingShapes(int basePos) //where does it go? left or right?
{
    //square
    if(sortshape == 0)
    {
      
  for (basePos = basePos; basePos <= 0; basePos += 1)
  {// goes from 0 degrees to desired degrees
    // in steps of 1 degree
    base.write(basePos);              // tell servo to go to position in variable 'pos'
    delay(100);                      // waits 15ms for the servo to reach the position
  }
    }

  else if (sortshape == 1)
  {
    //triangle
  for (basePos = basePos; basePos <= 180; basePos += 1)
    {// goes from where it is at to 180 degrees
    // in steps of 1 degree
    base.write(basePos);              // tell servo to go to position in variable 'pos'
    delay(100);                      // waits 15ms for the servo to reach the position
    }
  }
}

//slow down there motors!!
void motorStartingPosition()
{
  int basePos = 0; //variable to store the servo positon
  for (basePos = 0; basePos <= baseAngle; basePos += 1)
  {// goes from 0 degrees to desired degrees
    // in steps of 1 degree
    base.write(basePos);              // tell servo to go to position in variable 'pos'
    delay(100);                      // waits 15ms for the servo to reach the position
  }

  int shoulderPos = 90; //variable to store the servo positon
  for (shoulderPos = 90; shoulderPos <= shoulderAngle; shoulderPos += 1)
  {// goes from 90 degrees to desired degrees
    // in steps of 1 degree
   shoulder.write(shoulderPos);              // tell servo to go to position in variable 'pos'
    delay(100);                      // waits 15ms for the servo to reach the position
  }

  int armPos = 20; //variable to store the servo positon
  for (armPos = 20; armPos <= armAngle; armPos += 1)
  {// goes from 10 degrees to desired degrees
    // in steps of 1 degree
    arm.write(armPos);              // tell servo to go to position in variable 'pos'
    delay(100);                      // waits 15ms for the servo to reach the position
  }
  
}

void sendSuspendCmd()
{
  // send the suspend-true command
  Serial.println("<S1>");
}

void sendEnableCmd()
{
  // send the suspend-false command
  Serial.println("<S0>");
}

void sendCoordinatesToPC()
{
  // send the point data to the PC
  Serial.print("<P");
  Serial.print(xposition);
  Serial.print(",");
  Serial.print(yposition);
  Serial.print(",");
  Serial.print(sortshape);
  Serial.println(">");
}


//read the coordinates
void getDataFromPC() 
{
  if(Serial.available() > 0) 
  {
    char x = Serial.read();
    
    // the order of these IF clauses is significant
    if (x == endMarker) 
    {
      readInProgress = false;
      newDataFromPC = true;
      inputBuffer[bytesRecvd] = 0;
      coordinates[0] = inputBuffer[0]; //shape
      coordinates[1] = inputBuffer[1]; //x coordinate
      coordinates[2] = inputBuffer[2]; //y coordinate
    }
  
    if(readInProgress) 
    {
      inputBuffer[bytesRecvd] = x;
      bytesRecvd ++;
      if (bytesRecvd == buffSize) 
      {
        bytesRecvd = buffSize - 1;
      }
    }
  
    if (x == startMarker) 
    {
      bytesRecvd = 0;
      readInProgress = true;
    }
  }
}
