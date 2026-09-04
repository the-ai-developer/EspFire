#include "sensors/air_quality.h"
#include "config.h"

void AirQuality::begin() {
    pinMode(PIN_MQ2_AO, INPUT);
    analogReadResolution(12);
    bootAt_ = millis();
}

void AirQuality::update() {
    raw_ = analogRead(PIN_MQ2_AO);
}

int AirQuality::raw() const { return raw_; }

bool AirQuality::preheated() const {
    return millis() - bootAt_ >= MQ_PREHEAT_MS;
}

bool AirQuality::bad() const {
    return preheated() && raw_ >= AQ_ALERT_RAW;
}
