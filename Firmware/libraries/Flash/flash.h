#define ESP_LED_PIN 33

class Flash {
  public:
    static void begin() {
      pinMode(ESP_LED_PIN, OUTPUT);
      digitalWrite(ESP_LED_PIN, HIGH);
    }

    static void flash() {
      flash(10);
    }

    static void flash(int repeat) {
      for (int flashCount = 0; flashCount < repeat; flashCount++) {
        digitalWrite(ESP_LED_PIN, LOW);
        delay(100);
        digitalWrite(ESP_LED_PIN, HIGH);
        delay(100);
      }
    }
};
