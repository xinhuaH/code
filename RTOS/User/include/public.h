#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#define BK_SUCCESS   0
#define BK_FAILED    1

#define BK_TRUE      1
#define BK_FALSE     0

#define BK_RUNNING   1
#define BK_STOPPING  0

#define BK_MODE_IN   1
#define BK_MODE_OUT  2

#define BK_SHELTER         2
#define BK_UN_SHELTER      1
#define BK_SHELTER_UNKNOW  0

#define OPTOCOUPLER_LEVEL      1
#define OPTOCOUPLER_VERTICAL   2
#define OPTOCOUPLER_INJECTOR   3
#define OPTOCOUPLER_MOBILE     4

#define MOTOR_LEVEL            1
#define MOTOR_VERTICAL         2
#define MOTOR_INJECTOR1        3
#define MOTOR_INJECTOR2        4

#define MOTOR_UP               1
#define MOTOR_DOWN             2
#define MOTOR_LEFT             3
#define MOTOR_RIGHT            4

#define MOTOR_MOVE_COMPLETE    0
#define MOTOR_SPEED_FACTOR     1
#define MOTOR_MAX_MOVING_TIME  10000

#define MOTOR_START            1
#define MOTOR_STOP             0

#define SAMPLING_VERTICAL_MOTOR_ORIGIN_SPEED       1
#define SAMPLING_VERTICAL_MOTOR_ORIGIN_PER_MOVING  1
#define SAMPLING_VERTICAL_MOTOR_ORIGIN_MAX_PACES   200
#define SAMPLING_VERTICAL_ORIGIN_EXTRA_PACES 3
#define SAMPLING_VERTICAL_MOTOR_SWAY_SPEED         1
#define SAMPLING_VERTICAL_SWAY_PACES  6

#define SAMPLING_LEVEL_MOTOR_ORIGIN_SPEED       1
#define SAMPLING_LEVEL_MOTOR_ORIGIN_PER_MOVING  1
#define SAMPLING_LEVEL_MOTOR_ORIGIN_MAX_PACES   600
#define SAMPLING_LEVEL_ORIGIN_EXTRA_PACES 3
#define SAMPLING_LEVEL_MOTOR_SWAY_SPEED         1
#define SAMPLING_LEVEL_SWAY_PACES  6

#define INTERRUPT_HANDLER_NUM         5
#define INTERRUPT_HANDLER_BUFFER_SIZE 10

void executeDelay(unsigned int uiUs);

#endif
