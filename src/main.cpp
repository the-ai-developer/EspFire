#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "sensors/dht22_sensor.h"
#include "sensors/air_quality.h"
#include "display/lcd16x2.h"
#include "indicators/led_buzzer.h"
#include "core/env_state.h"
#include "cloud/blynk_client.h"

DHT22Sensor dht;
AirQuality air;
LCD16x2 lcd;
LedBuzzer indicators;
EnvState env;
BlynkClient cloud;

unsigned long lastLog = 0;

void setup() {
    Serial.begin(SERIAL_BAUD);
    delay(300);
    Serial.println("\n[EnvMonitor] Booting — DHT22 + MQ2 + LCD16x2");
    Serial.printf("Pins DHT:%d MQ-AO:%d LCD SDA:%d SCL:0x%02X RED:%d GREEN:%d BUZ:%d\n",
        PIN_DHT, PIN_MQ2_AO, PIN_LCD_SDA, PIN_LCD_SCL,
        PIN_RED_LED, PIN_GREEN_LED, PIN_BUZZER);

    dht.begin();
    air.begin();
    lcd.begin();
    indicators.begin();
    cloud.begin();

    Serial.println("[EnvMonitor] Ready. Green=Normal, Red+Buzzer=Abnormal");
    Serial.println("[EnvMonitor] MQ2 preheating ~2min — AQ ignored until ready");
}

void loop() {
    dht.update();
    air.update();

    DHTReading r = dht.reading();
    env.update(dht.tempHigh(), dht.humHigh(), air.bad());

    indicators.setWifiConnected(cloud.isConnected() || WiFi.status() == WL_CONNECTED);
    indicators.update(env.abnormal() ? SystemState::Fire : SystemState::Safe);
    lcd.update(r, air.raw(), env.abnormal(), WiFi.status() == WL_CONNECTED);
    cloud.update(env.abnormal() ? SystemState::Fire : SystemState::Safe, air.raw());

    if (env.changed()) {
        Serial.printf("[%lu] STATE -> %s (%s) | T:%.1f H:%.1f AQ:%d blynk:%s\n",
            millis(),
            env.abnormal() ? "ABNORMAL" : "NORMAL",
            env.reason(),
            r.tempC, r.humPct, air.raw(),
            cloud.isConnected() ? "ok" : "offline");
    }

    if (millis() - lastLog > 5000) {
        lastLog = millis();
        Serial.printf("[%lu] heartbeat | %s T:%.1f H:%.1f AQ:%d preheat:%s blynk:%s\n",
            millis(),
            env.abnormal() ? "ABNORMAL" : "NORMAL",
            r.tempC, r.humPct, air.raw(),
            air.preheated() ? "done" : "warming",
            cloud.isConnected() ? "connected" : "disconnected");
    }
}
