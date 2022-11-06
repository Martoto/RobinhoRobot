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
/*

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel pixels(8, 7, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pixels.begin();
  pixels.clear();
  pixels.show();

  pinMode(10, OUTPUT);  // configura  o pino como saída
  
    pinMode(3, OUTPUT);  // configura  o pino como saída

  pinMode(5, OUTPUT);  // configura  o pino como saída
  pinMode(6, OUTPUT);  // configura  o pino como saída
  pinMode(9, OUTPUT);  // configura  o pino como saída

  //*  pinMode(interruptPin, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);
  //  pinMode(interruptPin, INPUT_PULLUP);
 // attachInterrupt(digitalPinToInterrupt(interruptPin), blink, CHANGE);

}

char pwm1 = 100;

void loop() {
  char teste = ' ';
  analogWrite(3, pwm1);
  
  analogWrite(10, pwm1);
  analogWrite(5, pwm1);
  analogWrite(6, pwm1);
  analogWrite(9, pwm1);



  if(Serial.available())
  {
    teste = Serial.read();
  }

  if(teste == 'g')
  {
    for(int i=0; i<8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      pixels.show();   // Send the updated pixel colors to the hardware
    }
    Serial.write(teste);
  }
  else if(teste == 'r')
  {
    for(int i=0; i<8; i++) {
      pixels.setPixelColor(i, pixels.Color(150, 0, 0));
      pixels.show();   // Send the updated pixel colors to the hardware
    }
    Serial.write(teste);
  }
  else if(teste == 'b')
  {
    for(int i=0; i<8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 150));
      pixels.show();   // Send the updated pixel colors to the hardware
    }
    Serial.write(teste);
  }
  else if(teste == 'p')
  {
    uint16_t r, g, b, c, colorTemp, lux;
    tcs.getRawData(&r, &g, &b, &c);
    uint8_t cr = r;
    Serial.print(cr);
  }
  else if(teste == 'u')
  {
    pwm1 = (pwm1 + 50) % 255;
    Serial.print(teste);
  }
  else if(teste != ' ')
  {
    Serial.write(teste);
  }



  teste = ' ';
/*
  uint16_t r, g, b, c, colorTemp, lux;
  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  myserial.print("Color Temp: "); myserial.print(colorTemp, DEC); myserial.print(" K - ");
  myserial.print("Lux: "); myserial.print(lux, DEC); myserial.print(" - ");
  myserial.print("R: "); myserial.print(r, DEC); myserial.print(" ");
  myserial.print("G: "); myserial.print(g, DEC); myserial.print(" ");
  myserial.print("B: "); myserial.print(b, DEC); myserial.print(" ");
  myserial.print("C: "); myserial.print(c, DEC); myserial.print(" ");
  myserial.println(" ");
}
*/