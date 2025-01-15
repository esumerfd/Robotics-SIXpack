#include <servo_motion.h>
#include <pwm_servo.h>

ServoMotionRandom::ServoMotionRandom() {
  /* srand(); */
}

void ServoMotionRandom::move(PWMServo *servo) {
  // rand()
  servo->move();
};
