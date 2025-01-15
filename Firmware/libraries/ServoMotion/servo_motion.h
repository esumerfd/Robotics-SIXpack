#ifndef SERVO_MOTION_H
#define SERVO_MOTION_H

class PWMServo;

#include <pwm_servo.h>

/*
 * To be used in conjunction with setting a servos next motion.
 * - pwm.setPWM(0, 0, 1800);
 * Maintains state of the next move.
 */
class ServoMotion {
  public:
    virtual void move(PWMServo *servo);
};

class ServoMotionRandom : public ServoMotion {
  public:
    ServoMotionRandom();
    void move(PWMServo *servo);
};

#endif
