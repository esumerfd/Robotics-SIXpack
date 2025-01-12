#include "Arduino.h"
#include <flash.h>
#include <logging.h>
#include <servo.h>

void setup() {
  Log::begin();
  Log::println("SIXpack Debug");

  Servo::begin();
}

void loop() {
  Servo::move();

  Flash::flash();

  delay(10);
}

