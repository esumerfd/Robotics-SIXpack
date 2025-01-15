#ifndef PWM_SERVO_H
#define PWM_SERVO_H

class ServoMotion;
class ServoMotionRandom;

#include <Adafruit_PWMServoDriver.h>
#include <servo_motion.h>

class PWMServo {
  public:
    static void begin();
    static void begin(ServoMotionRandom *mover);
    static void move();
};

#endif
