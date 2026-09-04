#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "sensors/flame_sensor.h"
#include "indicators/led_buzzer.h"
#include "core/fire_state.h"
#include "cloud/blynk_client.h"

FlameSensor sensor;
LedBuzzer indicators;
FireState fireState;
BlynkClient cloud;

unsigned long lastLog = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(300);
    Serial.println("\n[EspFire] Booting — Pro Build | Blynk + QC0050");
    Serial.printf("Pins DO:%d AO:%d RED:%d GREEN:%d BUZ:%d\n",
        PIN_FLAME_DO, PIN_FLAME_AO, PIN_RED_LED, PIN_GREEN_LED, PIN_BUZZER);

    sensor.begin();
    indicators.begin();
    cloud.begin();

    Serial.println("[EspFire] Ready. Green=SAFE, Red+Buzzer=FIRE");
    Serial.println("[EspFire] Tip: adjust QC0050 pot if too sensitive to sunlight");
}

void loop() {
    sensor.update();
    fireState.update(sensor.isFire());
    indicators.setWifiConnected(cloud.isConnected() || WiFi.status() == WL_CONNECTED);
    indicators.update(fireState.state());
    cloud.update(fireState.state(), sensor.analogValue());

    if (fireState.changed()) {
        Serial.printf("[%lu] STATE -> %s | raw:%d analog:%d blynk:%s\n",
            millis(),
            fireState.isFire() ? "FIRE" : "SAFE",
            sensor.rawFire() ? 1 : 0,
            sensor.analogValue(),
            cloud.isConnected() ? "ok" : "offline");
    }

    if (millis() - lastLog > 5000) {
        lastLog = millis();
        Serial.printf("[%lu] heartbeat | state:%s analog:%d blynk:%s\n",
            millis(),
            fireState.isFire() ? "FIRE" : "SAFE",
            sensor.analogValue(),
            cloud.isConnected() ? "connected" : "disconnected");
    }
}
