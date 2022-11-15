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

volatile byte m1e_count = 0;
volatile byte m2e_count = 0;
volatile unsigned long m1e_last = 0;
volatile unsigned long m2e_last = 0;


//Altere o numero abaixo de acordo com o seu disco encoder
const unsigned int pulsos_por_volta = 20;

void m1e_i() {
  m1e_count++;
  m1e_last = millis();
}

void m2e_i() {
  m2e_count++;
  m2e_last = millis();
}

void setLock() {
  analogWrite(m1p1, 255);
  analogWrite(m1p2, 255);
  analogWrite(m2p1, 255);
  analogWrite(m2p2, 255);
}

void setVelocity(int motor1, int motor2) {
  motor1 = constrain(motor1, 0, 255);
  motor2 = constrain(motor2, 0, 255);
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
    myservo.write(2250);
    resp = cmd;
  } else if (cmd == 'c') {
    myservo.write(750);
    resp = cmd;
  } else if (cmd == 'g') {
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
  } else if (cmd == 'b') {
    for (int i = 0; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color(0, 0, 150));
      pixels.show();  // Send the updated pixel colors to the hardware
    }
    resp = cmd;
  } else if (cmd == 'p') {
    uint16_t r, g, b, c, colorTemp, lux;
    colorTemp = colorTemp;
    lux = lux;
    tcs.getRawData(&r, &g, &b, &c);
    uint8_t cr = r;
    resp = cr;
  } else {
    resetGpio();
    resp = cmd;
  }
  return resp;
}

void disable_encoder_interrupts() {
  detachInterrupt(digitalPinToInterrupt(m1e));
  detachInterrupt(digitalPinToInterrupt(m2e));
}
void enable_encoder_interrupts() {
  attachInterrupt(digitalPinToInterrupt(m1e), m1e_i, FALLING);
  attachInterrupt(digitalPinToInterrupt(m2e), m2e_i, FALLING);
}

void setup() {
  Serial.begin(9600);

  pinMode(m1p1, OUTPUT);
  pinMode(m1p2, OUTPUT);
  pinMode(m2p1, OUTPUT);
  pinMode(m2p2, OUTPUT);
  //myservo.attach(servo);
  pixels.begin();


  pinMode(m1e, INPUT_PULLUP);
  pinMode(m2e, INPUT_PULLUP);
  enable_encoder_interrupts();
  resetGpio();
}

void loop() {
  int base = 0;
  int dj = 0;
  unsigned int m1e_total_count = 0;
  unsigned int m2e_total_count = 0;
  unsigned long timeold = 0;
  int b = 70;

  int mstate = 0;
  unsigned long mstate_timer = 0;

  while (1) {
    char cmd, resp;
    if (Serial.available()) {
      cmd = Serial.read();
      resp = cmdTreatment(cmd);
      Serial.write(resp);
    }

    switch (mstate) {
      case 0:

        if (millis() - mstate_timer > 2000) {
          mstate = 1;
          mstate_timer = millis();
          m1e_total_count = 0;
          m2e_total_count = 0;
          m1e_count = 0;
          m2e_count = 0;

          base = 0;
          dj = 0;
          pixels.setPixelColor(0, pixels.Color(200, 0, 0));
          Serial.println("0-1");
          setVelocity(115, 115);
        }
        break;


      case 1:
        if ((m1e_count >= 2) && (m2e_count >= 2)) {
          timeold = millis();
          m1e_count = 0;
          m2e_count = 0;
          setVelocity(b, b);

          Serial.println("1-2");
          mstate = 2;
          mstate_timer = millis();
        } else if (millis() - mstate_timer > 100) {
          int m1 = (m1e_count >= 2) ? 80 : 150;
          int m2 = (m2e_count >= 2) ? 80 : 150;
          setVelocity(m1, m2);
        }
        break;


      case 2:
        //if ((m1e_total_count + m1e_count + m2e_total_count + m2e_count) >= 28) {
        if (max(m1e_total_count + m1e_count, m2e_total_count + m2e_count) >= 15) {
          base = 0;
          dj = 0;
          m1e_total_count = 0;
          m2e_total_count = 0;
          m1e_count = 0;
          m2e_count = 0;
          timeold = 0;

          mstate = 3;
          mstate_timer = millis();
          setLock();
          Serial.println("2-3");
          pixels.setPixelColor(0, pixels.Color(0, 200, 0));

        } else {
          disable_encoder_interrupts();

          const int BASETIME = 150;
          if ((millis() - m1e_last > BASETIME) || (millis() - m2e_last > BASETIME)) {
            base += 8*max((millis() - m1e_last)/BASETIME, (millis() - m2e_last)/BASETIME);
          }

          base = constrain(base, 0, 255);

          m1e_total_count += m1e_count;
          m2e_total_count += m2e_count;

          dj = (m2e_total_count - m1e_total_count) * 25;
          //dj = constrain(dj, -40, 40);

          if (dj > 0) {
            setVelocity(base + b + dj, base + b);
          } else {
            setVelocity(base + b, base + b - dj);
          }

          Serial.print("PWM = ");
          Serial.println(base, DEC);
          Serial.print("dj = ");
          Serial.println(dj, DEC);
          Serial.print("C1 = ");
          Serial.println(m1e_count, DEC);
          Serial.print("C2 = ");
          Serial.println(m2e_count, DEC);
          Serial.print("T1 = ");
          Serial.println(m1e_total_count, DEC);
          Serial.print("T2 = ");
          Serial.println(m2e_total_count, DEC);

          m1e_count = 0;
          m2e_count = 0;

          enable_encoder_interrupts();

          Serial.println(millis(), DEC);

          //basevel = (basevel + 3) % 255;
        }
        break;
      case 3:
        if (millis() - mstate_timer > 1000) {
          mstate = 0;
          mstate_timer = millis();
          pixels.setPixelColor(0, pixels.Color(0, 0, 200));
          setVelocity(0, 0);
          Serial.println("3-0");
        }
        break;
    }
  }
}