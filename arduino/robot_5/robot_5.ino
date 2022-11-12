#include "ServoTimer2.h"
#include <SoftwareSerial.h>
#include "pins.h"
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel pixels(8, 13, NEO_GRB + NEO_KHZ800);
ServoTimer2 myservo;

int motor1_velocity = 0;
int motor2_velocity = 0;
int test = 0;

volatile byte m1e_count = 0;
volatile byte m2e_count = 0;

int m1rpm;
int m2rpm;

unsigned long timeold;

//Altere o numero abaixo de acordo com o seu disco encoder
unsigned int pulsos_por_volta = 20;

void m1e_i() {
  m1e_count++;
}

void m2e_i() {
  m2e_count++;
}

void setVelocity(int motor1, int motor2) {
  if (motor1 == 0) {
    analogWrite(m1p1, 0);
    analogWrite(m1p2, 0);
  } else if (motor1 > 0) {
    analogWrite(m1p1, motor1);
    analogWrite(m1p2, 0);
  } else if (motor1 < 0) {
    analogWrite(m1p1, 0);
    analogWrite(m1p2, -motor1);
  }

  if (motor2 == 0) {
    analogWrite(m2p1, 0);
    analogWrite(m2p2, 0);
  } else if (motor2 > 0) {
    analogWrite(m2p1, motor2);
    analogWrite(m2p2, 0);
  } else if (motor2 < 0) {
    analogWrite(m2p1, 0);
    analogWrite(m2p2, -motor2);
  }
}

void resetGpio() {
  pixels.clear();
  pixels.show();
  setVelocity(0, 0);
  myservo.write(2250);
}

char cmdTreatment(char cmd) {
  char resp;
  if (cmd == 'w') {
    motor1_velocity = 200;
    motor2_velocity = 200;
    resp = cmd;
  }
  else if (cmd == 's') {
    motor1_velocity = -200;
    motor2_velocity = -200;
    resp = cmd;
  }
  else if (cmd == 'd') {
    motor1_velocity = -200;
    motor2_velocity = 200;
    resp = cmd;
  }
  else if (cmd == 'a') {
    motor1_velocity = 200;
    motor2_velocity = -200;
    resp = cmd;
  }
  else if (cmd == 'o') {
    myservo.write(2250);
    resp = cmd;
  }
  else if (cmd == 'c') {
    myservo.write(750);
    resp = cmd;
  }
  else if (cmd == 'g') {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      pixels.show();  // Send the updated pixel colors to the hardware
    }
    resp = cmd;
  }
  else if (cmd == 'r') {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(150, 0, 0));
      pixels.show();  // Send the updated pixel colors to the hardware
    }
    resp = cmd;
  }
  else if (cmd == 'b') {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 150));
      pixels.show();  // Send the updated pixel colors to the hardware
    }
    resp = cmd;
  }
  else if (cmd == 'p') {
    uint16_t r, g, b, c, colorTemp, lux;
    tcs.getRawData(&r, &g, &b, &c);
    uint8_t cr = r;
    resp = cr;
  }
  else {
    resetGpio();
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
  pixels.begin();

  
  pinMode(m1e, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(m1e), m1e_i, FALLING);
  pinMode(m2e, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(m2e), m2e_i, FALLING);
  resetGpio();
}
  int basevel = 0;


int dj = 0;
void loop() {
  
  int delay_time = 2000;
  char cmd, resp;
  if (Serial.available()) {
    cmd = Serial.read();
    resp = cmdTreatment(cmd);
    Serial.write(resp);
  }


  //Atualiza contador a cada segundo
  if (millis() - timeold >= 300)
  {
    //Desabilita interrupcao durante o calculo
    detachInterrupt(digitalPinToInterrupt(m1e));
    detachInterrupt(digitalPinToInterrupt(m2e));
    
    m1rpm = (60 * 300 / pulsos_por_volta ) / (millis() - timeold) * m1e_count;
    m2rpm = (60 * 300 / pulsos_por_volta ) / (millis() - timeold) * m2e_count;
     // cmdVelMsg.linear.x = (-1)*cstP*deltaX + cstI*(deltaX*(time-last_time)) + cstD*(deltaX-last_deltaX)/(time-last_time);    
     /*
    int deltarpm1 = m1rpm-120;
    int last_deltarpm1;
    int cstP = 1;
    int cstI = 0;
    int cstD = 0;
    int m1out = (-1)*cstP*deltarpm1 + cstI*(deltarpm1*(millis()-timeold)) + cstD*(deltarpm1-last_deltarpm1)/(millis()-timeold);    
    last_deltarpm1 = deltarpm1;

    int deltarpm1 = m1rpm-120;
    int last_deltarpm1;
    int cstP = 1;
    int cstI = 0;
    int cstD = 0;
    int m1out = (-1)*cstP*deltarpm1 + cstI*(deltarpm1*(millis()-timeold)) + cstD*(deltarpm1-last_deltarpm1)/(millis()-timeold);    
    last_deltarpm1 = deltarpm1;
*/

if(!m1rpm && !m2rpm){
  bse+=5;
}

   dj+= 1*(m1rpm-m2rpm);
dj = constrain(dj,-30, 30)   ;
  if (dj > 0){
    setVelocity(60+dj, 60);
  } else {
    setVelocity(60, 60-dj);
  }

    timeold = millis();
    
    m1e_count = 0;
    m2e_count = 0;

    //Mostra o valor de RPM no serial monitor
    Serial.print("PWM = ");
    Serial.println(basevel, DEC);
        Serial.print("dj = ");
    Serial.println(dj, DEC);

    Serial.print("RPM1 = ");
    Serial.println(m1rpm, DEC);
    Serial.print("RPM2 = ");
    Serial.println(m2rpm, DEC);
    
    //Habilita interrupcao
    attachInterrupt(digitalPinToInterrupt(m1e), m1e_i, FALLING);
    attachInterrupt(digitalPinToInterrupt(m2e), m2e_i, FALLING);

    //basevel = (basevel + 3) % 255;
  }

  //test = (test * 58 / 13) * 45 / 4 + 34;
  //delay(delay_time);
}