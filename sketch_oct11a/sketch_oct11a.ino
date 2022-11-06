#include <Servo.h>

#define red 12
#define green 8
#define blue 13
#define SERVO 11

Servo s;
int i, teste;
void setup() {
  // put your setup code here, to run once:
  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);

  s.attach(SERVO);
  s.write(0);
  i = 0;
  teste = 0;
}

void loop() {
  int dela = 500;
  s.write(0);
  pinMode(red, HIGH);
  pinMode(green, LOW);
  pinMode(blue, LOW);
  delay(dela);
  pinMode(red, LOW);
  pinMode(green, HIGH);
  pinMode(blue, LOW);
  delay(dela);
  pinMode(red, LOW);
  pinMode(green, LOW);
  pinMode(blue, HIGH);
  delay(dela);
  s.write(360);
  pinMode(red, HIGH);
  pinMode(green, LOW);
  pinMode(blue, LOW);
  delay(dela);
  pinMode(red, LOW);
  pinMode(green, HIGH);
  pinMode(blue, LOW);
  delay(dela);
  pinMode(red, LOW);
  pinMode(green, LOW);
  pinMode(blue, HIGH);
  delay(dela);
}
