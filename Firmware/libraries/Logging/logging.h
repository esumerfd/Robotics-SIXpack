class Log {
  public:
    static void begin() {
      Serial.begin(9600);
      delay(100);
    }

    static void println(const char* format, ...) {
      char buffer[100];
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, sizeof(buffer), format, args);
      va_end(args);
      Serial.println(buffer);
    }

    static void print(const char* format, ...) {
      char buffer[100];
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, sizeof(buffer), format, args);
      va_end(args);
      Serial.print(buffer);
    }
};
