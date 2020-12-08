/*
 *
 * Find nearest object by scanning infared sensor across 180 degrees
 * driven by a servo attached sensor. Once 180 degrees is scanned, point
 * sensor to nearest object and display results. Press button to re-scan.
 *
 * W. K. Rodiger 4/5/2014
 * W. K. Rodiger 12/8/20 Moved to PlatformIO
 *
 */

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Servo.h>

//SoftwareSerial mySerial(7, 8, true); // RX, TX
// This caused noise on the servo

// Servo constants
const int SERVO = 6;
const int SENS = 2;
const int BUTTON = 13;

//Shift Register Output
const int SER = 2;   //serial output to shift register
const int LATCH = 3; //shift register latch pin
const int CLK = 4;   //shift register clock pin

Servo myServo;
int angle = 0;
int dist = 0;
int min_dist = 0; // maximum sensor reading possible
int min_dist_angle = 30;
boolean scan = false;
boolean point = false;

void setup()
{
  //set pins as outputs
  pinMode(SER, OUTPUT);
  pinMode(LATCH, OUTPUT);
  pinMode(CLK, OUTPUT);

  //zero out displays
  digitalWrite(LATCH, LOW);
  shiftOut(SER, CLK, MSBFIRST, B00000000);
  digitalWrite(LATCH, HIGH);

  myServo.attach(SERVO);
  pinMode(BUTTON, INPUT);
  pinMode(SENS, INPUT);
  Serial.begin(9600);
  delay(1000); //wait for LCD to be ready
  lcdClear();
  lcdString(128, "Arduino - Locate"); //line 1 starts at memory location 128
  lcdString(192, "Closest Object:");  //line 2 starts at location 192
  delay(3000);
}

/**********************************************
 * LCD Library for driving the Scott Edwards  *
 * Serial LCD Backpack.                       *
 * Arduino version - WKR 4/1/2014             *
 **********************************************/

void lcdClear()
// Clear LCD and wait for display to respond
{
  Serial.write(254); //instruction
  Serial.write(1);   //clear
  delay(100);        //wait to complete
}

void lcdString(int pos, String string)
//position cursor and print
{
  Serial.write(254); //instruction
  Serial.write(pos); //position cursor
  Serial.print(string);
  delay(100);
}

void lcdInt(int pos, int val)
//position cursor and print
{
  Serial.write(254); //instruction
  Serial.write(pos); //position cursor
  Serial.print(val);
  delay(100);
}

void loop()
{
  if (!scan)
  { // have we scanned yet?
    // update display
    lcdClear();
    delay(1000);
    lcdString(128, "Arduino:Scanning");
    lcdString(192, "Ang:    Dst:    ");
    delay(1000);

    //scan across 180 degrees by 10 degree increments
    for (angle = 30; angle <= 150; angle = angle + 10)
    {
      // rotate sensor to angle
      myServo.write(angle);
      delay(150); //wait for servo to move

      //take reading of distance
      dist = analogRead(SENS);

      // update display
      lcdInt(196, angle);
      lcdString(204, "    "); // clear last number
      lcdInt(204, dist);
      delay(10);

      // is reading closest object?
      if (dist > min_dist)
      {
        min_dist_angle = angle; // set min dist angle
        min_dist = dist;        // closest distance
      }
    }
    scan = true; // we are done scanning
  }
  if (scan && !point)
  { // scanning done but not yet pointed to closest object

    // rotate sensor to angle of closest object
    myServo.write(min_dist_angle);
    delay(150); //wait for servo to move

    // update display
    lcdClear();
    lcdString(128, "Arduino: Found");
    lcdString(192, "Ang:    Dst:    ");
    lcdInt(196, min_dist_angle);
    lcdInt(204, min_dist);
    point = true; // done pointing
  }
  if (digitalRead(BUTTON) == HIGH)
  { // do it again
    scan = false;
    point = false;
    min_dist = 0; // maximum sensor reading possible
    min_dist_angle = 30;
  }
}