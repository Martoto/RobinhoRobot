#include "ServoTimer2.h"
#include <SoftwareSerial.h>
#include "pins.h"
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define BASE_PWM 60
#define TICKS_FORWARD 42
#define TICKS_ROTATE_90 8
#define LOCK_TIME 500
#define PWM_SURGE_MAX_ADJ 100
#define PWM_SURGE_ADJ_KP 35
#define PWM_SWAY_MAX_ADJ 30
#define PWM_SWAY_ADJ_KP 15
#define BASETIME 200
#define PWM_BASE_STUCK 10
#define STARTUP_TICKS 2
#define PWM_STARTUP_BOOST 150
#define PWM_STARTUP 90
#define STARTUP_BOOST_TIME 150
#define ENCODER_PPR 20


Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel pixels(8, 13, NEO_GRB + NEO_KHZ800);
ServoTimer2 myservo;

volatile byte m1e_count = 0;
volatile byte m2e_count = 0;
volatile uint32_t m1e_last = 0;
volatile uint32_t m2e_last = 0;

int16_t unstuck_boost = 0;
uint16_t m1e_total_count = 0;
uint16_t m2e_total_count = 0;

enum class mstate_e {
  STOPPED,
  STARTUP,
  MOVING,
  LOCKED,
  WAITING_POSE
};

mstate_e mstate = mstate_e::STOPPED;
uint32_t mstate_timer = 0;

enum class movement_type_e {
  FORWARD,
  LEFT,
  RIGHT,
  BACKWARDS
};

movement_type_e movement_type = movement_type_e::FORWARD;
// x y ang
uint8_t pose[3] = { 0 };
uint32_t pose_time = 0;

void m1e_i() {
  m1e_count++;
  m1e_last = millis();
}

void m2e_i() {
  m2e_count++;
  m2e_last = millis();
}

void setBrake() {
  analogWrite(m1p1, 250);
  analogWrite(m1p2, 250);
  analogWrite(m2p1, 250);
  analogWrite(m2p2, 250);
}

void setVelocity(int motor1, int motor2) {
  motor1 = constrain(motor1, -250, 250);
  motor2 = constrain(motor2, -250, 250);
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


void openServo() {
  myservo.write(2250);
}

void closeServo() {
  myservo.write(750);
}

void mt_setVelocity(int motor1, int motor2) {
  if (movement_type == movement_type_e::FORWARD) {
    setVelocity(motor1, motor2);
  } else if (movement_type == movement_type_e::LEFT) {
    setVelocity(motor1, -motor2);
  } else if (movement_type == movement_type_e::RIGHT) {
    setVelocity(-motor1, motor2);
  } else if (movement_type == movement_type_e::BACKWARDS) {
    setVelocity(-motor1, -motor2);
  }
}


void mstate_reset() {
  mstate = mstate_e::STOPPED;
  mstate_timer = millis();
  m1e_total_count = 0;
  m2e_total_count = 0;
  m1e_count = 0;
  m2e_count = 0;

  unstuck_boost = 0;
  setVelocity(0, 0);
}

void mstate_start(movement_type_e movetype) {
  mstate = mstate_e::STARTUP;
  mstate_timer = millis();
  m1e_total_count = 0;
  m2e_total_count = 0;
  m1e_count = 0;
  m2e_count = 0;

  unstuck_boost = 0;

  movement_type = movetype;
  mt_setVelocity(PWM_STARTUP, PWM_STARTUP);
}

void cmdTreatment(int cmd) {
  if (cmd == 0x1C) {
     mstate_start(movement_type_e::FORWARD);

/*
    static int iii = 0;
    if (iii % 4 == 0) {
      mstate_start(movement_type_e::FORWARD);
    } else if (iii % 4 == 1) {
      mstate_start(movement_type_e::LEFT);
    } else if (iii % 4 == 2) {
      if (iii % 8 == 0) {

        mstate_start(movement_type_e::BACKWARDS);
      } else {
        mstate_start(movement_type_e::FORWARD);
      }
    } else if (iii % 4 == 3) {
      mstate_start(movement_type_e::RIGHT);
    }
    iii++;
*/

  } else if (cmd == 0x1F) {
    mstate_start(movement_type_e::LEFT);
  } else if (cmd == 0x1E) {
    mstate_start(movement_type_e::RIGHT);
  } else if (cmd == 0x1D) {
    mstate_start(movement_type_e::BACKWARDS);
  } else {
    mstate_reset();

    if (cmd == 0x1B) {
      openServo();
      Serial.write('1');
    } else if (cmd == 0x1A) {
      closeServo();
      Serial.write('1');
    } else if ((cmd & 0x1F) == 0x1C) {
      for (int i = 0; i < 8; i++) {
        pixels.setPixelColor(i, pixels.Color((cmd & 0x80) ? 255 : 0, (cmd & 0x40) ? 255 : 0, (cmd & 0x20) ? 255 : 0));
        pixels.show();  // Send the updated pixel colors to the hardware
      }
      Serial.write('1');
    } else if (cmd == 0b11100000) {

      uint16_t r, g, b, c, colorTemp, lux;
      colorTemp = colorTemp;
      lux = lux;
      tcs.getRawData(&r, &g, &b, &c);

      if ((r > 150) && (g > 150) && (b > 150)) {
        Serial.write('0');
      } else if ((r > 150) && (g > 150)) {
        Serial.write('3');
      } else if (r > 150) {
        Serial.write('1');
      } else if (g > 150) {
        Serial.write('2');
      } else if (b > 150) {
        Serial.write('4');
      }
    } else if (cmd == 0b11100001) {
      // TODO read battery
      Serial.write('4');
    } else if (cmd == 0b00000000) {
      for (int j = 0; j < 3; j++) {
        while (!Serial.available()) {}
        pose[j] = Serial.read();
      }
      pose_time = millis();
      pixels.setPixelColor(6, pose[0], pose[1], pose[2]);
      pixels.show();
    }
  }
  return;
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
  myservo.attach(servo);
  pixels.begin();


  pinMode(m1e, INPUT_PULLUP);
  pinMode(m2e, INPUT_PULLUP);
  enable_encoder_interrupts();
  resetGpio();
}


void loop() {
  //closeServo();
  int iii = 0;
  pixels.setPixelColor(0, 150, 0, 0);
  pixels.show();

  while (1) {
    if (Serial.available()) {
      pixels.setPixelColor(iii % 4, 150, 0, 0, 0);
      pixels.setPixelColor((iii + 1) % 4, 0, 150, 0, 0);

      pixels.show();
      iii++;
      int cmd = Serial.read();
      cmdTreatment(cmd);
    }

    switch (mstate) {
      case mstate_e::STOPPED:
        break;

      case mstate_e::STARTUP:
        // Check if both motors started rotating and we can move to normal moving operation
        if ((m1e_count >= STARTUP_TICKS) && (m2e_count >= STARTUP_TICKS)) {
          mt_setVelocity(BASE_PWM, BASE_PWM);

          //Serial.println("1-2");
          mstate = mstate_e::MOVING;
          mstate_timer = millis();

          // Check if the startup is taking too long, and if yes, raise PWM even higher
        } else if (millis() - mstate_timer > STARTUP_BOOST_TIME) {
          int m1 = (m1e_count >= STARTUP_TICKS) ? BASE_PWM : PWM_STARTUP_BOOST;
          int m2 = (m2e_count >= STARTUP_TICKS) ? BASE_PWM : PWM_STARTUP_BOOST;
          mt_setVelocity(m1, m2);
        }
        break;

      case mstate_e::MOVING:
        // Check if we rotated enough
        // For forward/backwards
        if ((((m1e_total_count + m1e_count + m2e_total_count + m2e_count) >= TICKS_FORWARD) && (movement_type == movement_type_e::FORWARD || movement_type == movement_type_e::BACKWARDS))
            // For rotation
            || ((max(m1e_total_count + m1e_count, m2e_total_count + m2e_count) >= TICKS_ROTATE_90) && ((movement_type == movement_type_e::LEFT) || (movement_type == movement_type_e::RIGHT)))) {
          mstate = mstate_e::LOCKED;
          mstate_timer = millis();
          setBrake();

          // Run normal controller
        } else {
          disable_encoder_interrupts();

          // If either motor hasn't rotated at all for BASETIME and is probably stuck, increase the base PWM power for both, increasing with time stuck
          if ((millis() - m1e_last > BASETIME) || (millis() - m2e_last > BASETIME)) {
            unstuck_boost = PWM_BASE_STUCK * max((millis() - m1e_last) / BASETIME, (millis() - m2e_last) / BASETIME);
          }

          unstuck_boost = constrain(unstuck_boost, 0, 255);

          m1e_total_count += m1e_count;
          m2e_total_count += m2e_count;
          m1e_count = 0;
          m2e_count = 0;

          // Calculates the difference between pulses in each motor and muls by PWM_ADJ_KP
          int dj = (m2e_total_count - m1e_total_count) * ((movement_type == movement_type_e::FORWARD || movement_type == movement_type_e::BACKWARDS) ? PWM_SURGE_ADJ_KP : PWM_SWAY_ADJ_KP);
          // constrains the maximum left/right PWM adjustement
          int max_adj = ((movement_type == movement_type_e::FORWARD || movement_type == movement_type_e::BACKWARDS) ? PWM_SURGE_MAX_ADJ : PWM_SWAY_MAX_ADJ);
          dj = constrain(dj, -max_adj, max_adj);

          int m1 = BASE_PWM + unstuck_boost;
          int m2 = BASE_PWM + unstuck_boost;

          if (dj > 0) {
            m1 += dj;
          } else {
            m2 += -dj;
          }

          mt_setVelocity(m1, m2);

          enable_encoder_interrupts();
        }
        break;


      case mstate_e::LOCKED:
        if (millis() - mstate_timer > LOCK_TIME) {
          mstate = mstate_e::WAITING_POSE;
          setVelocity(0, 0);
        }
        break;

      case mstate_e::WAITING_POSE:
        //if (millis() - pose_time < 500) {
        {
          mstate_reset();
          pixels.setPixelColor(5, pose[0], pose[1], pose[2]);
          pixels.show();
          Serial.write('1');
        }
        break;
    }
  }
}
