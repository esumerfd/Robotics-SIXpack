#include "Arduino.h"
#include <flash.h>
#include <logging.h>
#include <pwm_servo.h>

void setup() {
  Log::begin();
  Log::println("SIXpack Debug");

  PWMServo::begin();
}

void loop() {
  PWMServo::move();

  Flash::flash();

  delay(100);
}

