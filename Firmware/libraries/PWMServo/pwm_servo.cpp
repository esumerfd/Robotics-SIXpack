#include <pwm_servo.h>
#include <servo_motion.h>

static Adafruit_PWMServoDriver pwm;

void PWMServo::begin() {
  /* begin(new ServerMotionRandom()) */
/* } */

/* static void begin(ServerMotionRandom *mover) { */

  ServerMotionRandom *mover = new ServerMotionRandom();

  pwm = Adafruit_PWMServoDriver();

  pwm.begin();
  pwm.setPWMFreq(1600); 
  pwm.setPWMFreq(60);
}

void PWMServo::move() {
  pwm.setPWM(0, 0, 1800);
};
