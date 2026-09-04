#pragma once
#include <Arduino.h>
#include "sensors/dht22_sensor.h"

class LCD16x2 {
public:
    void begin();
    void update(const DHTReading& dht, int aqRaw, bool abnormal, bool wifiOk);

private:
    unsigned long lastRefresh_ = 0;
    bool ready_ = false;
};
