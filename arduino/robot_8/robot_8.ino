#include <math.h>

//#include "ServoTimer2.h"
#include <SoftwareSerial.h>
#include "pins.h"
#include "cmd.h"
#include "Adafruit_TCS34725.h"
#include <Adafruit_NeoPixel.h>

#define BASE_PWM 60
#define TICKS_ROTATE_90 8
#define PULSES_PER_DISTANCE (42.0 / 25.0)
#define BRAKE_TIME 500
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
#define ANGLE_MOVE_TOLERANCE 10
#define SURGE_MOVE_TOLERANCE 5
#define PWM_MAX 250
#define GRID_SIZE 25


uint8_t initFlag = 0;

enum angle_e : int16_t {
  EAST = 0,
  SOUTH = 64,
  WEST = 128,
  NORTH = 192,
  INVALID = 1000
};

angle_e angle_e_rotate_left(angle_e in) {
  switch (in) {
    case EAST:
      return NORTH;
    case SOUTH:
      return EAST;
    case WEST:
      return SOUTH;
    case NORTH:
      return WEST;
    default:
      return INVALID;
  }
}

angle_e angle_e_rotate_right(angle_e in) {
  switch (in) {
    case EAST:
      return SOUTH;
    case SOUTH:
      return WEST;
    case WEST:
      return NORTH;
    case NORTH:
      return EAST;
    default:
      return INVALID;
  }
}

/*
const byte grid_colors[8][8] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

const byte return_to_base_dir[8][8] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

const byte blocked_moves[8][8] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};
*/

struct Angle {
  int16_t val = 0;

  Angle(int16_t val) {
    this->val = val;
  }

  int16_t diff(Angle b) {
    int16_t k = (this->val - b.val) % 256;
    return min(256 - k, k);
  }

  void opposite() {
    val = (val + 128) % 256;
  }

  angle_e nearest_direction() {
    angle_e ret;
    int16_t mdiff = 1000;

    int16_t calc;

    calc = abs(this->diff(Angle{ val: EAST }));
    if (calc < mdiff) {
      mdiff = calc;
      ret = EAST;
    }

    calc = abs(this->diff(Angle{ val: SOUTH }));
    if (calc < mdiff) {
      mdiff = calc;
      ret = SOUTH;
    }

    calc = abs(this->diff(Angle{ val: WEST }));
    if (calc < mdiff) {
      mdiff = calc;
      ret = WEST;
    }

    calc = abs(this->diff(Angle{ val: NORTH }));
    if (calc < mdiff) {
      mdiff = calc;
      ret = NORTH;
    }

    return ret;
  }
};

struct GridPosition;

struct RealPosition {
  int16_t x = 0;
  int16_t y = 0;

  RealPosition(int16_t x, int16_t y) {
    this->x = x;
    this->y = y;
  }

  GridPosition to_grid();
  double distance(RealPosition b);
  Angle angle_to(RealPosition b);
};

struct GridPosition {
  int16_t x = 0;
  int16_t y = 0;

  GridPosition(int16_t x, int16_t y) {
    this->x = x;
    this->y = y;
  }

  RealPosition to_real_center() {
    return RealPosition{ x: this->x * GRID_SIZE + (GRID_SIZE / 2), y: this->y * GRID_SIZE + (GRID_SIZE / 2) };
  }

  GridPosition move(angle_e direction) {
    switch (direction) {
      case NORTH:
        return GridPosition{ x: x, y: y - 1 };
      case WEST:
        return GridPosition{ x: x - 1, y: y };
      case EAST:
        return GridPosition{ x: x + 1, y: y };
      case SOUTH:
        return GridPosition{ x: x, y: y + 1 };
      default:
        return GridPosition{ -1, -1 };
    }
  }

  bool isMoveValid(angle_e direction) {
    GridPosition newpos = this->move(direction);
    if (newpos.x < 0 || newpos.y < 0) {
      return false;
    }
    if (newpos.x > 9 || newpos.y > 8) {
      return false;
    }
    if (newpos.x < 4 && newpos.y < 4) {
      return false;
    }
    return true;  // TODO walls
  }
};

GridPosition RealPosition::to_grid() {
  return GridPosition{ x: this->x / 8, y: this->y / 8 };
}

double RealPosition::distance(RealPosition b) {
  return sqrt(sq(this->x - b.x) + sq(this->y - b.y));
}
Angle RealPosition::angle_to(RealPosition b) {
  float radians = atan2(this->y - b.y, this->x - b.x);
  Angle ret = Angle{ val: static_cast<int16_t>(256.0 * radians / (2 * 3.14159)) % 256 };
  ret.opposite();  // https://en.cppreference.com/w/cpp/numeric/math/atan2
  return ret;
}

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);
Adafruit_NeoPixel pixels(8, 13, NEO_GRB + NEO_KHZ800);
//ServoTimer2 myservo;

volatile byte m1e_count = 0;
volatile byte m2e_count = 0;
volatile uint32_t m1e_last = 0;
volatile uint32_t m2e_last = 0;

int16_t unstuck_boost = 0;
uint16_t m12e_target = 0;
uint16_t m1e_total_count = 0;
uint16_t m2e_total_count = 0;
Angle movement_final_angle = 0;

enum class mstate_e {
  STOPPED,
  STARTUP,
  MOVING,
  BRAKES,
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

enum class overmstate_e {
  STOPPED,
  PURE_ROTATION,
  FORWARD,
  BACKWARDS
};

overmstate_e overmstate = overmstate_e::STOPPED;
RealPosition overmstate_target_realpos = RealPosition{ 0, 0 };

// x y ang
RealPosition pose_real = { 0, 0 };
Angle pose_ang = { 0 };
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
  analogWrite(m1p1, PWM_MAX);
  analogWrite(m1p2, PWM_MAX);
  analogWrite(m2p1, PWM_MAX);
  analogWrite(m2p2, PWM_MAX);
}

void setVelocity(int motor1, int motor2) {
  motor1 = constrain(motor1, -PWM_MAX, PWM_MAX);
  motor2 = constrain(motor2, -PWM_MAX, PWM_MAX);
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
  //myservo.write(2250);
}

void openServo() {
  //myservo.write(2250);
}

void closeServo() {
  //myservo.write(750);
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

// ??????
void mstate_start(movement_type_e movetype, int16_t target);
void mstate_start(movement_type_e movetype, int16_t target) {
  mstate = mstate_e::STARTUP;
  mstate_timer = millis();
  m12e_target = target;
  m1e_total_count = 0;
  m2e_total_count = 0;
  m1e_count = 0;
  m2e_count = 0;

  unstuck_boost = 0;


  movement_type = movetype;
  mt_setVelocity(PWM_STARTUP, PWM_STARTUP);
}

void mstate_rotation_to(Angle target);
void mstate_rotation_to(Angle target) {
  movement_final_angle = target;
  int16_t diff = pose_ang.diff(target);
  int16_t encoder_pulses = (TICKS_ROTATE_90 * abs(diff)) / (256 / 4);
  mstate_start((diff < 0) ? movement_type_e::RIGHT : movement_type_e::LEFT, encoder_pulses);
}

void overmstate_reset() {
  overmstate = overmstate_e::STOPPED;
  overmstate_target_realpos = RealPosition{ 0, 0 };
  mstate_reset();
}

void overmstate_surge_run() {
  // Position good?
  if (pose_real.distance(overmstate_target_realpos) < SURGE_MOVE_TOLERANCE) {
    // Finish angle good?
    if (abs(pose_ang.diff(movement_final_angle)) < ANGLE_MOVE_TOLERANCE) {
      pixels.setPixelColor(4, 0, 0, 100);
      pixels.show();
      overmstate_reset();
      Serial.write('1');
    } else {
      pixels.setPixelColor(4, 0, 100, 0);
      pixels.show();
      mstate_rotation_to(movement_final_angle);
    }
  } else {
    Angle target_move_angle = pose_real.angle_to(overmstate_target_realpos);
    if (overmstate == overmstate_e::BACKWARDS) {
      target_move_angle.opposite();
    }
    // Move angle good?
    if (abs(pose_ang.diff(target_move_angle)) < ANGLE_MOVE_TOLERANCE) {
      pixels.setPixelColor(4, 100, 100, 100);
      pixels.show();//193,68
      int16_t pulses = PULSES_PER_DISTANCE * pose_real.distance(overmstate_target_realpos);
      mstate_start(overmstate == overmstate_e::FORWARD ? movement_type_e::FORWARD : movement_type_e::BACKWARDS, pulses);
    } else {
      pixels.setPixelColor(4, 100, 0, 0);
      pixels.show();
      mstate_rotation_to(target_move_angle);
    }
  }
}

void overmstate_start_surge(overmstate_e overmstate_in);
void overmstate_start_surge(overmstate_e overmstate_in) {
  movement_final_angle = pose_ang.nearest_direction();
  if (!pose_real.to_grid().isMoveValid(pose_ang.nearest_direction())) {
    pixels.setPixelColor(6, 0, 100, 100);
    pixels.show();
    overmstate_reset();
    Serial.write('1');
    return;
  }

  pixels.setPixelColor(6, 0, 0, 100);
  pixels.show();
  overmstate = overmstate_in;

  overmstate_target_realpos = pose_real.to_grid().move(pose_ang.nearest_direction()).to_real_center();
  overmstate_surge_run();
}

void overmstate_start_yaw(bool right) {
  pixels.setPixelColor(6, 0, 100, 0);
  pixels.show();
  overmstate = overmstate_e::PURE_ROTATION;
  angle_e target_direction = right ? angle_e_rotate_right(pose_ang.nearest_direction()) : angle_e_rotate_left(pose_ang.nearest_direction());
  mstate_rotation_to(Angle{ target_direction });
}

void cmdTreatment(int cmd) {
  // TODO
  if (cmd == CMD_POSE) {
    Serial.write('1');
    int16_t posein[3];
    for (int j = 0; j < 3; j++) {
      while (!Serial.available()) {}
      posein[j] = Serial.read();
      //Serial.write('');
    }
    pose_real = RealPosition{ posein[0], posein[1] };
    pose_ang = Angle{ posein[2] };
    pose_time = millis();
    pixels.setPixelColor(4, posein[0], posein[1], posein[2]);
    pixels.show();
    initFlag = 1;
    Serial.write('!');
  } else if (!initFlag) {
    return;
  } else if (cmd ==  CMD_FORWARD) {
    overmstate_start_surge(overmstate_e::FORWARD);
  } else if (cmd == CMD_LEFT) {
    overmstate_start_yaw(false);
  } else if (cmd == CMD_RIGHT) {
    overmstate_start_yaw(true);
  } else if (cmd == CMD_BACKWARD) {
    overmstate_start_surge(overmstate_e::BACKWARDS);
  } else {
    Serial.write('?');
    mstate_reset();

    if (cmd == 0x1B) {
      openServo();
      Serial.write('1');
    } else if (cmd == 0x1A) {
      closeServo();
      Serial.write('1');
    } else if ((cmd & 0x1F) == 0x18) {
      int i = 7;
      //for (int i = 7; i < 8; i++) {
      pixels.setPixelColor(i, pixels.Color((cmd & 0x80) ? 255 : 0, (cmd & 0x40) ? 255 : 0, (cmd & 0x20) ? 255 : 0));
      pixels.show();  // Send the updated pixel colors to the hardware
      //}
      Serial.write('1');
    } else if (cmd == 0b11100000) {
      Serial.write('1');
      // TODO
      //Serial.write(grid_colors[GridPosition.current()]);
      /*
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
      */
    } else if (cmd == 0b11100001) {
      // TODO read battery
      Serial.write('4');
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
  //myservo.attach(servo);
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
  for (int i = 1; i < 7; i++) {
    pixels.setPixelColor(i, 0, 0, 0);
  }
  pixels.show();

  while (1) {
    if (Serial.available()) {
      pixels.setPixelColor(iii % 3, 150, 0, 0, 0);
      pixels.setPixelColor((iii + 1) % 3, 0, 150, 0, 0);

      pixels.show();
      iii++;
      int cmd = Serial.read();
      cmdTreatment(cmd);
    }

    switch (mstate) {
      case mstate_e::STOPPED:
        pixels.setPixelColor(5, 150, 0, 0);
        pixels.show();
        break;

      case mstate_e::STARTUP:
        pixels.setPixelColor(5, 0, 150, 0);
        pixels.show();
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
        pixels.setPixelColor(5, 0, 0, 150);
        pixels.show();
        // Check if we rotated enough
        // For forward/backwards
        if ((((m1e_total_count + m1e_count + m2e_total_count + m2e_count) >= m12e_target) && (movement_type == movement_type_e::FORWARD || movement_type == movement_type_e::BACKWARDS))
            // For rotation
            || ((max(m1e_total_count + m1e_count, m2e_total_count + m2e_count) >= m12e_target) && ((movement_type == movement_type_e::LEFT) || (movement_type == movement_type_e::RIGHT)))) {
          mstate = mstate_e::BRAKES;
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


      case mstate_e::BRAKES:
        pixels.setPixelColor(5, 150, 0, 150);
        pixels.show();
        if (millis() - mstate_timer > BRAKE_TIME) {
          mstate = mstate_e::WAITING_POSE;
          mstate_timer = millis();
          setVelocity(0, 0);
        }
        break;

      case mstate_e::WAITING_POSE:
        pixels.setPixelColor(5, 150, 150, 150);
        pixels.show();

        if (pose_time > mstate_timer) {
          switch (overmstate) {
            case overmstate_e::FORWARD:
            case overmstate_e::BACKWARDS:
              overmstate_surge_run();

              break;
            case overmstate_e::PURE_ROTATION:
              if (abs(pose_ang.diff(movement_final_angle)) < ANGLE_MOVE_TOLERANCE) {
                pixels.setPixelColor(7, 0, 100, 0);
                pixels.show();
                overmstate_reset();
                Serial.write('1');
              } else {
                pixels.setPixelColor(7, 100, 0, 0);
                pixels.show();
                mstate_rotation_to(movement_final_angle);
              }
              break;
            default:
              overmstate_reset();
              break;
          }
        }
        break;
    }
  }
}