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

void setVelocity(int motor1, int motor2) {
  if (motor1 == 0) {
    digitalWrite(m1p1, LOW);
    digitalWrite(m1p2, LOW);
    //Serial.print("\tmotor1: LOW LOW");
  } else if (motor1 > 0) {
    analogWrite(m1p1, motor1);
    digitalWrite(m1p2, LOW);
    //Serial.print("\tmotor1: HIGH LOW");
  } else if (motor1 < 0) {
    digitalWrite(m1p1, LOW);
    analogWrite(m1p2, -motor1);
    //Serial.print("\tmotor1: LOW HIGH");
  }

  if (motor2 == 0) {
    digitalWrite(m2p1, LOW);
    digitalWrite(m2p2, LOW);
    //Serial.print("\tmotor2: LOW LOW");
  } else if (motor2 > 0) {
    analogWrite(m2p1, motor2);
    digitalWrite(m2p2, LOW);
    //Serial.print("\tmotor2: HIGH LOW");
  } else if (motor2 < 0) {
    digitalWrite(m2p1, LOW);
    analogWrite(m2p2, -motor2);
    //Serial.print("\tmotor2: LOW HIGH");
  }
}

void setServo(int value) {
  myservo.write(value);
  Serial.print("\t servo:");
  Serial.println(value);
}

void openServo() {
  setServo(750);
}

void closeServo() {
  setServo(2250);
}


void reseGpio() {
  setVelocity(0, 0);
  openServo();
}

char cmdTreatment(char cmd) {
  char resp;
  if (cmd == 'w') {
    motor1_velocity = 200;
    motor2_velocity = 200;
    resp = cmd;
  } else if (cmd == 's') {
    motor1_velocity = -200;
    motor2_velocity = -200;
    resp = cmd;
  } else if (cmd == 'd') {
    motor1_velocity = -200;
    motor2_velocity = 200;
    resp = cmd;
  } else if (cmd == 'a') {
    motor1_velocity = 200;
    motor2_velocity = -200;
    resp = cmd;
  } else if (cmd == 'o') {
    openServo();
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      pixels.show();  // Send the updated pixel colors to the hardware
    }
    resp = cmd;
  } else if (cmd == 'c') {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(150, 0, 0));
      pixels.show();  // Send the updated pixel colors to the hardware
    }

    closeServo();
    resp = cmd;
  } else if (cmd == 't') {
    Serial.print(test);
    resp = cmd;
  } else if ((cmd == 'g')) {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 150, 0));
      pixels.show();  // Send the updated pixel colors to the hardware
    }
    resp = cmd;
  } else if (cmd == 'r') {
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
  } else if (cmd == 'p') {
    uint16_t r, g, b, c, colorTemp, lux;
    tcs.getRawData(&r, &g, &b, &c);
    uint8_t cr = r;
    resp = cr;
  } else {
    reseGpio();
    resp = cmd;
  }
  return resp;
}

volatile int m1e_count = 0;
void m1e_i() {
  m1e_count++;
}

volatile int m2e_count = 0;
void m2e_i() {
  m2e_count++;
}

void setup() {
  Serial.begin(9600);

  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  myservo.attach(servo);

  reseGpio();

  Serial.begin(9600);
  pixels.begin();
  pixels.clear();
  pixels.show();

  pinMode(m1e, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(m1e), m1e_i, CHANGE);
  pinMode(m2e, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(m2e), m2e_i, CHANGE);
}

void loop() {
  //int delay_time = 2000;

  char cmd, resp;

  unsigned long t1, t2;
  t1 = micros();
  if (Serial.available()) {
    t2 = micros();
    /*
Serial.print("avay yes ");
    Serial.print(t2-t1);
Serial.print(" ");
    Serial.print(t1);
    Serial.print(" ");
    Serial.println(t2);
*/

    t1 = micros();
    cmd = Serial.read();
    t2 = micros();
    /*
    Serial.print("sere ");
        Serial.print(t2-t1);
Serial.print(" ");
    Serial.print(t1);
    Serial.print(" ");
    Serial.println(t2);
*/
    t1 = micros();
    resp = cmdTreatment(cmd);
    t2 = micros();
    /*
    Serial.print("secm ");
        Serial.print(t2-t1);
Serial.print(" ");

    Serial.print(t1);
    Serial.print(" ");
    Serial.println(t2);
 */
    t1 = micros();
    Serial.write(resp);
    t2 = micros();
    /*
    Serial.print("sewr ");
    Serial.print(t2-t1);
Serial.print(" ");

    Serial.print(t1);
    Serial.print(" ");
    Serial.println(t2);    
  */
    delay(2000);
  } else {
    t2 = micros();
  }
  /*
  Serial.print("avan ");
    Serial.print(t2-t1);
Serial.print(" ");

  Serial.print(t1);
  Serial.print(" ");
  Serial.println(t2);
*/
  t1 = micros();
  test = (test * 58 / 13) * 45 / 4 + 34;
  t2 = micros();
  /*
  Serial.print("math ");
    Serial.print(t2-t1);
Serial.print(" ");

  Serial.print(t1);
  Serial.print(" ");
  Serial.println(t2);
*/
  t1 = micros();
  setVelocity(motor1_velocity, motor2_velocity);
  t2 = micros(); /*
    Serial.print("setv ");
    Serial.print(t2-t1);
Serial.print(" ");

  Serial.print(t1);
  Serial.print(" ");
  Serial.println(t2);
*/
  /* square
  setVelocity(150,150)
  delay(600);
  setVelocity(150,-150);
  delay(300);
  */
  //delay(delay_time);
}