#ifndef PWM_SERVO_H
#define PWM_SERVO_H

#include <Adafruit_PWMServoDriver.h>

class PWMServo {
  public:
    static void begin();
    /* static void begin(ServerMotionRandom *mover); */
    static void move();
};

#endif
