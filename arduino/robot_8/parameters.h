// overmstate
#define ANGLE_PER_PULSE 9
#define MIN_ANGLE_ROT 15
#define PULSES_PER_DISTANCE (1.4)
#define ANGLE_MOVE_TOLERANCE 8
#define SURGE_MOVE_EUCLIDEAN_TOLERANCE 6
//#define SURGE_MOVE_FORWARD_TOLERANCE 2

//mstate
///startup
#define STARTUP_TICKS 1
#define PWM_STARTUP_BOOST 150
#define PWM_STARTUP 90
#define STARTUP_BOOST_TIME 300
///moving
#define PWM_SURGE_MAX_ADJ 100
#define PWM_SURGE_ADJ_KP 35
#define PWM_SWAY_MAX_ADJ 30
#define PWM_SWAY_ADJ_KP 15
#define BASE_PWM 60
#define BASETIME 300
#define PWM_BASE_STUCK 10
///brake
#define BRAKE_TIME 500
#define PWM_BRAKE 250

//other
//#define ENCODER_PPR 20
#define PWM_MAX 250
#define GRID_SIZE 25
