#include <SoftwareSerial.h>

#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#include"ServoTimer2.h"

ServoTimer2 servo1;
void setup() 
{
servo1.attach(11);   
}

void loop() 
{                 // put your main code here, to run repeatedly:
servo1.write(750);  //min pulse width for 0 degree
delay(1000);
servo1.write(1500);  //pulse width for 90 degree
delay(1000);
servo1.write(2250);  //max pulse width for around 180 degree
delay(1000);
}
