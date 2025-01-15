#include <pwm_servo.h>
#include <servo_motion.h>

static Adafruit_PWMServoDriver pwm;

void PWMServo::begin() {
  PWMServo::begin(new ServoMotionRandom());
}

void PWMServo::begin(ServoMotionRandom *mover) {
  pwm = Adafruit_PWMServoDriver();

  pwm.begin();
  pwm.setPWMFreq(1600); 
  pwm.setPWMFreq(60);
}

void PWMServo::move() {
  pwm.setPWM(0, 0, 1800);
};
