#pragma once
#include <Arduino.h>

struct DHTReading {
    float tempC = NAN;
    float humPct = NAN;
    bool valid = false;
};

class DHT22Sensor {
public:
    void begin();
    void update();
    DHTReading reading() const;
    bool tempHigh() const;
    bool humHigh() const;

private:
    DHTReading last_;
    unsigned long lastRead_ = 0;
};
