#include "sensors/flame_sensor.h"
#include "config.h"

void FlameSensor::begin() {
    pinMode(PIN_FLAME_DO, INPUT);
    analogReadResolution(12);
}

void FlameSensor::update() {
    int rawDigital = digitalRead(PIN_FLAME_DO);
    raw_ = FLAME_ACTIVE_LOW ? (rawDigital == LOW) : (rawDigital == HIGH);
    analog_ = analogRead(PIN_FLAME_AO);
    unsigned long now = millis();

    if (raw_) {
        if (fireSince_ == 0) fireSince_ = now;
        clearSince_ = 0;
        if (!fire_ && (now - fireSince_ >= FIRE_DEBOUNCE_MS)) {
            fire_ = true;
        }
    } else {
        if (clearSince_ == 0) clearSince_ = now;
        fireSince_ = 0;
        if (fire_ && (now - clearSince_ >= FIRE_CLEAR_MS)) {
            fire_ = false;
        }
        if (!fire_) {
            fireSince_ = 0;
        }
    }
}

bool FlameSensor::isFire() const { return fire_; }
int FlameSensor::analogValue() const { return analog_; }
bool FlameSensor::rawFire() const { return raw_; }
