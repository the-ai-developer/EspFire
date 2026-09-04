#pragma once
#include <Arduino.h>

class AirQuality {
public:
    void begin();
    void update();
    int raw() const;
    bool bad() const;
    bool preheated() const;

private:
    int raw_ = 0;
    unsigned long bootAt_ = 0;
};
