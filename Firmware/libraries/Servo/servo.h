#include <Adafruit_PWMServoDriver.h>

static Adafruit_PWMServoDriver pwm;

class Servo {
  public:
    static void begin() {
      pwm = Adafruit_PWMServoDriver();

      pwm.begin();
      pwm.setPWMFreq(1600); 
      pwm.setPWMFreq(60);
    }

    static void move() {
      pwm.setPWM(0, 0, 1800);
    }
};


