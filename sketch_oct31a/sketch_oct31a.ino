#include <Servo.h>
#include <SoftwareSerial.h>

#define servo 3
#define m1p1 5
#define m1p2 6
#define m2p1 10
#define m2p2 9

Servo myservo;
SoftwareSerial espSerial(13, 12);

void setup() {
  
  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  
  myservo.attach(servo);
  myservo.write(0);
  
  Serial.begin(9600);
  espSerial.begin(9600);
  delay(10);
  espSerial.write('a');
  delay(10);
}

void loop() {
  int delay_time = 2000;
  char teste;
  
  if(espSerial.available())
  {
    teste = espSerial.read();
    delay(1);
    Serial.println(teste);
    espSerial.write(teste);
  }
  if(teste == 'w')
  {
    digitalWrite(m1p1, HIGH);
    digitalWrite(m1p2, LOW);
    digitalWrite(m2p1, HIGH);
    digitalWrite(m2p2, LOW);
  }
  else if(teste == 's')
  {
    digitalWrite(m1p1, LOW);
    digitalWrite(m1p2, HIGH);
    digitalWrite(m2p1, LOW);
    digitalWrite(m2p2, HIGH);
  }
  else if(teste == 'd')
  {
    digitalWrite(m1p1, LOW);
    digitalWrite(m1p2, HIGH);
    digitalWrite(m2p1, HIGH);
    digitalWrite(m2p2, LOW);
  }
  else if(teste == 'a')
  {
    digitalWrite(m1p1, HIGH);
    digitalWrite(m1p2, LOW);
    digitalWrite(m2p1, LOW);
    digitalWrite(m2p2, HIGH);
  }
  else
  {
    digitalWrite(m1p1, LOW);
    digitalWrite(m1p2, LOW);
    digitalWrite(m2p1, LOW);
    digitalWrite(m2p2, LOW);
  }
  delay(delay_time);
}