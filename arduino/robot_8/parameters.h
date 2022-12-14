// overmstate
#define ANGLE_PER_PULSE 10
#define MIN_ANGLE_ROT 17
#define PULSES_PER_DISTANCE (1.25)
#define ANGLE_MOVE_TOLERANCE 7
#define SURGE_MOVE_EUCLIDEAN_TOLERANCE 6
#define SURGE_MOVE_FORWARD_TOLERANCE 1
#define SURGE_MOVE_FORWARD_ANGLE_TOLERANCE 15


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

//color
#define COLOR_HIGH_RED 80
#define COLOR_HIGH_GREEN 50
#define COLOR_HIGH_BLUE 20
#define COLOR_LOW 0

//Servo
#define OPEN_SERVO 1700
#define CLOSE_SERVO 900

//Grid
#define GRID_SIZE_SMALL 25
#define GRID_SIZE_LARGE 50
#define GRID_SMALL_X_REAL_START 75
#define GRID_SMALL_Y_REAL_START 0
#define GRID_SMALL_X_GRID_END 5
#define GRID_LARGE_X_REAL_START 25
#define GRID_LARGE_Y_REAL_START 100
#define GRID_LARGE_Y_GRID_START 4
#define GRID_LARGE_X_GRID_END 3
#define GRID_LARGE_Y_GRID_END 5
