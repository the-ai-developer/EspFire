#include "indicators/led_buzzer.h"
#include "config.h"

void LedBuzzer::begin() {
    pinMode(PIN_RED_LED, OUTPUT);
    pinMode(PIN_GREEN_LED, OUTPUT);
    pinMode(PIN_BUZZER, OUTPUT);
    digitalWrite(PIN_RED_LED, LOW);
    digitalWrite(PIN_GREEN_LED, HIGH);
    digitalWrite(PIN_BUZZER, LOW);
}

void LedBuzzer::update(SystemState state) {
    unsigned long now = millis();
    if (state == SystemState::Safe) {
        digitalWrite(PIN_GREEN_LED, HIGH);
        digitalWrite(PIN_RED_LED, LOW);
        digitalWrite(PIN_BUZZER, LOW);
        ledOn_ = false;
        buzzerOn_ = false;
        return;
    }

    digitalWrite(PIN_GREEN_LED, LOW);

    if (now - lastLedToggle_ >= LED_BLINK_MS) {
        lastLedToggle_ = now;
        ledOn_ = !ledOn_;
        digitalWrite(PIN_RED_LED, ledOn_ ? HIGH : LOW);
    }

    if (now - lastBuzzerToggle_ >= BUZZER_PULSE_MS) {
        lastBuzzerToggle_ = now;
        buzzerOn_ = !buzzerOn_;
        digitalWrite(PIN_BUZZER, buzzerOn_ ? HIGH : LOW);
    }
}
