#include "sensors/dht22_sensor.h"
#include "config.h"
#include <DHT.h>

static DHT dht(PIN_DHT, DHT22);

void DHT22Sensor::begin() {
    pinMode(PIN_DHT, INPUT_PULLUP);
    dht.begin();
}

void DHT22Sensor::update() {
    unsigned long now = millis();
    if (now - lastRead_ < DHT_READ_MS) return;
    lastRead_ = now;
    float h = dht.readHumidity();
    float t = dht.readTemperature();
    if (!isnan(h) && !isnan(t)) {
        last_.humPct = h;
        last_.tempC = t;
        last_.valid = true;
    }
}

DHTReading DHT22Sensor::reading() const { return last_; }

bool DHT22Sensor::tempHigh() const {
    return last_.valid && last_.tempC >= TEMP_ALERT_C;
}

bool DHT22Sensor::humHigh() const {
    return last_.valid && last_.humPct >= HUM_ALERT_PCT;
}
