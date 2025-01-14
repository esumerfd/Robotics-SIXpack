#include "Arduino.h"
#include <flash.h>
#include <logging.h>

class PWMServo;
class ServerMotion;
class ServerMotionRandom;

#include <pwm_servo.h>
#include <servo_motion.h>

void setup() {
  Log::begin();
  Log::println("SIXpack Debug");

  PWMServo::begin();
  /* PWMServo::begin(new ServerMotionRandom()); */
}

void loop() {
  PWMServo::move();

  Flash::flash();

  delay(100);
}

