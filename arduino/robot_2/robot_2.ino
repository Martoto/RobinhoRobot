#include <Servo.h>
#include <SoftwareSerial.h>
#include "pins.h"

Servo myservo;

void setVelocity(int motor1, int motor2)
{
  if(motor1 == 0){
    digitalWrite(m1p1, LOW);
    digitalWrite(m1p2, LOW);
  }
  else if(motor1 > 0){
    analogWrite(m1p1, 255);
    digitalWrite(m1p2, LOW);
  }
  else if(motor1 < 0){
    digitalWrite(m1p1, LOW);
    analogWrite(m1p2, 255);
  }

  if(motor2 == 0){
    digitalWrite(m2p1, LOW);
    digitalWrite(m2p2, LOW);
  }
  else if(motor2 > 0){
    analogWrite(m2p1, 255);
    digitalWrite(m2p2, LOW);
  }
  else if(motor2 < 0){
    digitalWrite(m2p1, LOW);
    analogWrite(m2p2, 255);
  }
}

void setServo(int angle)
{
  myservo.write(angle);
  Serial.print("\t servo:");
  Serial.println(angle);
}

void reseGpio()
{
  setVelocity(0,0);
  setServo(0);
}

char cmdTreatment(char cmd){
  char resp;
  if(cmd == 'w'){
    setVelocity(1, 1);
    resp = cmd;
  }
  else if(cmd == 's'){
    setVelocity(-1, -1);
    resp = cmd;
  }
  else if(cmd == 'd'){
    setVelocity(-1, 1);
    resp = cmd;
  }
  else if(cmd == 'a'){
    setVelocity(1, -1);
    resp = cmd;
  }
  else if(cmd == 'o'){
    setServo(0);
    resp = cmd;
  }
  else if(cmd == 'c'){
    setServo(180);
    resp = cmd;
  }
  else{
    reseGpio();
    resp = cmd;
  }
  return resp;
}

void setup() {
  Serial.begin(9600);

  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  myservo.attach(servo);

  reseGpio();
}

void loop(){
  int delay_time = 2000;
  char cmd, resp;
  if(Serial.available()){
    cmd = Serial.read();
    resp = cmdTreatment(cmd);
    Serial.write(resp);
  }
  delay(delay_time);
}
