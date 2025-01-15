#include "Arduino.h"
#include <flash.h>
#include <logging.h>

#include <pwm_servo.h>
#include <servo_motion.h>

void setup() {
  Log::begin();
  Log::println("SIXpack Debug");

  PWMServo::begin(new ServoMotionRandom());
}

void loop() {
  PWMServo::move();

  Flash::flash();

  delay(100);
}

