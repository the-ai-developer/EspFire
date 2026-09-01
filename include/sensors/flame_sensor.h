#pragma once
#include <Arduino.h>

class FlameSensor {
public:
    void begin();
    void update();
    bool isFire() const;
    int analogValue() const;
    bool rawFire() const;

private:
    bool fire_ = false;
    int analog_ = 0;
    bool raw_ = false;
    unsigned long fireSince_ = 0;
    unsigned long clearSince_ = 0;
};
