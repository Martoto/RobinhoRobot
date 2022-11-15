#include "ServoTimer2.h"
#include <SoftwareSerial.h>
#include "pins.h"
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define BASE_PWM 60
#define TICKS_FORWARD 42
#define TICKS_ROTATE_90 13
#define LOCK_TIME 500
#define PWM_MAX_ADJ 30
#define PWM_ADJ_KP 10
#define BASETIME 200
#define PWM_BASE_STUCK 8
#define STARTUP_TICKS 2
#define PWM_STARTUP_BOOST 150
#define STARTUP_BOOST_TIME 150
#define ENCODER_PPR 20

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel pixels(8, 13, NEO_GRB + NEO_KHZ800);
ServoTimer2 myservo;

volatile byte m1e_count = 0;
volatile byte m2e_count = 0;
volatile unsigned long m1e_last = 0;
volatile unsigned long m2e_last = 0;


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
  motor1 = constrain(motor1, -255, 255);
  motor2 = constrain(motor2, -255, 255);
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
    resp = cmd;
  } else if (cmd == 's') {
    resp = cmd;
  } else if (cmd == 'd') {
    resp = cmd;
  } else if (cmd == 'a') {
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

void mstate_start_advance(int distance) {

}

void loop() {
  int base = 0;
  int dj = 0;
  unsigned int m1e_total_count = 0;
  unsigned int m2e_total_count = 0;
  unsigned long timeold = 0;
  
  // 0 is forward
  // 10 is rotate 90
  int mstate = 10;
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
          Serial.println("0-1");
          setVelocity(100, 100);
        }
        break;

      case 1:
        if ((m1e_count >= STARTUP_TICKS) && (m2e_count >= STARTUP_TICKS)) {
          timeold = millis();
          m1e_count = 0;
          m2e_count = 0;
          setVelocity(BASE_PWM, BASE_PWM);

          Serial.println("1-2");
          mstate = 2;
          mstate_timer = millis();
        } else if (millis() - mstate_timer > STARTUP_BOOST_TIME) {
          int m1 = (m1e_count >= STARTUP_TICKS) ? BASE_PWM : PWM_STARTUP_BOOST;
          int m2 = (m2e_count >= STARTUP_TICKS) ? BASE_PWM : PWM_STARTUP_BOOST;
          setVelocity(m1, m2);
        }
        break;

      case 2:
        if ((m1e_total_count + m1e_count + m2e_total_count + m2e_count) >= TICKS_FORWARD) {
        //if (max(m1e_total_count + m1e_count, m2e_total_count + m2e_count) >= 15) {
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
          //setVelocity(0, 0);
          Serial.println("2-3");

        } else {
          disable_encoder_interrupts();

          if ((millis() - m1e_last > BASETIME) || (millis() - m2e_last > BASETIME)) {
            base += PWM_BASE_STUCK*max((millis() - m1e_last)/BASETIME, (millis() - m2e_last)/BASETIME);
          }

          base = constrain(base, 0, 255);

          m1e_total_count += m1e_count;
          m2e_total_count += m2e_count;

          dj = (m2e_total_count - m1e_total_count) * PWM_ADJ_KP;
          dj = constrain(dj, -PWM_MAX_ADJ, PWM_MAX_ADJ);

          if (dj > 0) {
            setVelocity(base + BASE_PWM + dj, base + BASE_PWM);
          } else {
            setVelocity(base + BASE_PWM, base + BASE_PWM - dj);
          }

          m1e_count = 0;
          m2e_count = 0;

          enable_encoder_interrupts();
        }
        break;

      case 3:
        if (millis() - mstate_timer > LOCK_TIME) {
          mstate = 10;
          mstate_timer = millis();
          setVelocity(0, 0);
          Serial.println("3-0");
        }
        break;




      case 10:
        if (millis() - mstate_timer > 2000) {
          mstate = 11;
          mstate_timer = millis();
          m1e_total_count = 0;
          m2e_total_count = 0;
          m1e_count = 0;
          m2e_count = 0;

          base = 0;
          dj = 0;
          Serial.println("0-1");
          setVelocity(100, -100);
        }
        break;

      case 11:
        if ((m1e_count >= STARTUP_TICKS) && (m2e_count >= STARTUP_TICKS)) {
          timeold = millis();
          m1e_count = 0;
          m2e_count = 0;
          setVelocity(BASE_PWM, BASE_PWM);

          Serial.println("1-2");
          mstate = 12;
          mstate_timer = millis();
        } else if (millis() - mstate_timer > STARTUP_BOOST_TIME) {
          int m1 = (m1e_count >= STARTUP_TICKS) ? BASE_PWM : PWM_STARTUP_BOOST;
          int m2 = (m2e_count >= STARTUP_TICKS) ? -BASE_PWM : -PWM_STARTUP_BOOST;
          setVelocity(m1, m2);
        }
        break;

      case 12:
        //if ((m1e_total_count + m1e_count + m2e_total_count + m2e_count) >= TICKS_ROTATE_90) {
        if (max(m1e_total_count + m1e_count, m2e_total_count + m2e_count) >= 7) {
          base = 0;
          dj = 0;
          m1e_total_count = 0;
          m2e_total_count = 0;
          m1e_count = 0;
          m2e_count = 0;
          timeold = 0;

          mstate = 13;
          mstate_timer = millis();
          setLock();
          //setVelocity(0, 0);
          Serial.println("2-3");

        } else {
          disable_encoder_interrupts();

          if ((millis() - m1e_last > BASETIME) || (millis() - m2e_last > BASETIME)) {
            base += PWM_BASE_STUCK*max((millis() - m1e_last)/BASETIME, (millis() - m2e_last)/BASETIME);
          }

          base = constrain(base, 0, 255);

          m1e_total_count += m1e_count;
          m2e_total_count += m2e_count;

          dj = (m2e_total_count - m1e_total_count) * PWM_ADJ_KP;
          dj = constrain(dj, -PWM_MAX_ADJ, PWM_MAX_ADJ);

          if (dj > 0) {
            setVelocity(base + BASE_PWM + dj, -(base + BASE_PWM));
          } else {
            setVelocity(base + BASE_PWM, -(base + BASE_PWM - dj));
          }

          m1e_count = 0;
          m2e_count = 0;

          enable_encoder_interrupts();
        }
        break;

      case 13:
        if (millis() - mstate_timer > LOCK_TIME) {
          mstate = 10;
          mstate_timer = millis();
          setVelocity(0, 0);
          Serial.println("3-0");
        }
        break;

    }
  }
}