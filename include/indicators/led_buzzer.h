#pragma once
#include <Arduino.h>

enum class SystemState { Safe, Fire };

class LedBuzzer {
public:
    void begin();
    void update(SystemState state);

private:
    unsigned long lastLedToggle_ = 0;
    unsigned long lastBuzzerToggle_ = 0;
    bool ledOn_ = false;
    bool buzzerOn_ = false;
};
