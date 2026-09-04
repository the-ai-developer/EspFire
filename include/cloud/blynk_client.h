#pragma once
#include <Arduino.h>
#include "indicators/led_buzzer.h"

class BlynkClient {
public:
    void begin();
    void update(SystemState state, int analogVal);
    void updateEnv(SystemState state, int aqRaw, float tempC, float humPct);
    bool isConnected() const;

private:
    void connect();
    unsigned long lastNotify_ = 0;
    unsigned long lastSend_ = 0;
    unsigned long lastEnvSend_ = 0;
    bool notifiedFire_ = false;
};
