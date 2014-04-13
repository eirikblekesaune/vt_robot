//speed bytes
#define FULL_STEP 0x0
#define HALF_STEP 0x1
#define QUARTER_STEP 0x2
#define EIGHT_STEP 0x3
#define SIXTEENTH_STEP 0x7
#define STOP_SPEED 0
#define MAX_POSITION 550
#define MIN_POSITION 0
#define NO_TARGET -1
#define UP 0
#define DOWN 1

//parameter enum
enum param {
  kParamDirection,
  kParamSpeed,
  kParamEnable,
  kParamInterval
};
typedef enum _state_t {
	AT_MAX_POSTITION,
	AT_MIN_POSTITION,
	AT_TARGET,
	GOING_UP,
	GOING_DOWN,
	GOING_TO_TARGET,
} state_t;


void stateChanged(state_t newState);
