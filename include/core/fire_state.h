#pragma once
#include "indicators/led_buzzer.h"

class FireState {
public:
    void update(bool flameFire);
    SystemState state() const;
    bool changed() const;
    bool isFire() const;

private:
    SystemState state_ = SystemState::Safe;
    bool changed_ = false;
};
