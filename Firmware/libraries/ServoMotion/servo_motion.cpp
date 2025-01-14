#include <servo_motion.h>
#include <pwm_servo.h>

ServerMotionRandom::ServerMotionRandom() {
  /* srand(); */
}

void ServerMotionRandom::move(PWMServo servo) {
  // rand()
  servo.move();
};
