#include "cloud/blynk_client.h"
#include "config.h"

#if __has_include("secrets.h")
#include "secrets.h"
#else
#define WIFI_SSID "WIFI_SSID_PLACEHOLDER"
#define WIFI_PASS "WIFI_PASS_PLACEHOLDER"
#define BLYNK_AUTH_TOKEN "BLYNK_TOKEN_PLACEHOLDER"
#define BLYNK_TEMPLATE_ID "TMPL_EspFire"
#define BLYNK_TEMPLATE_NAME "EspFire"
#endif

#ifndef BLYNK_TEMPLATE_ID
#define BLYNK_TEMPLATE_ID "TMPL_EspFire"
#endif
#ifndef BLYNK_TEMPLATE_NAME
#define BLYNK_TEMPLATE_NAME "EspFire"
#endif
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

void BlynkClient::begin() {
    WiFi.mode(WIFI_STA);
    connect();
}

void BlynkClient::connect() {
    if (String(WIFI_SSID) == "WIFI_SSID_PLACEHOLDER") {
        Serial.println("[WiFi] secrets.h not set — skipping WiFi");
        return;
    }
    Serial.printf("[WiFi] Connecting to %s ...\n", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 8000) {
        delay(250);
        Serial.print(".");
    }
    Serial.println();
    if (WiFi.status() == WL_CONNECTED) {
        Serial.printf("[WiFi] Connected IP %s RSSI %d\n", WiFi.localIP().toString().c_str(), WiFi.RSSI());
        Blynk.config(BLYNK_AUTH_TOKEN);
        Blynk.connect(3000);
        Serial.printf("[Blynk] %s\n", Blynk.connected() ? "connected" : "connect failed");
    } else {
        Serial.printf("[WiFi] Failed status %d — check SSID/pass or hotspot on\n", WiFi.status());
    }
}

bool BlynkClient::isConnected() const {
    return Blynk.connected();
}

void BlynkClient::update(SystemState state, int analogVal) {
    if (String(WIFI_SSID) == "WIFI_SSID_PLACEHOLDER") return;

    if (WiFi.status() != WL_CONNECTED) {
        WiFi.reconnect();
        return;
    }

    Blynk.run();

    unsigned long now = millis();
    if (now - lastSend_ < 1000) return;
    lastSend_ = now;

    if (Blynk.connected()) {
        Blynk.virtualWrite(BLYNK_VPIN_FIRE, state == SystemState::Fire ? 1 : 0);
        Blynk.virtualWrite(BLYNK_VPIN_ANALOG, analogVal);
        Blynk.virtualWrite(BLYNK_VPIN_UPTIME, (int)(now / 1000));
    }

    if (state == SystemState::Fire && !notifiedFire_) {
        if (now - lastNotify_ >= BLYNK_NOTIFY_COOLDOWN_MS) {
            if (Blynk.connected()) {
                Blynk.logEvent("fire_alert", "Fire detected! Check immediately.");
            }
            lastNotify_ = now;
            notifiedFire_ = true;
        }
    }

    if (state == SystemState::Safe && notifiedFire_) {
        notifiedFire_ = false;
        if (Blynk.connected()) {
            Blynk.logEvent("fire_cleared", "Fire cleared — system safe.");
        }
    }

    if (!Blynk.connected() && now - lastNotify_ > 5000) {
        Blynk.connect(2000);
    }
}

void BlynkClient::updateEnv(SystemState state, int aqRaw, float tempC, float humPct) {
    update(state, aqRaw);

    unsigned long now = millis();
    if (now - lastEnvSend_ < 2000) return;
    lastEnvSend_ = now;

    if (Blynk.connected() && !isnan(tempC) && !isnan(humPct)) {
        Blynk.virtualWrite(BLYNK_VPIN_TEMP, tempC);
        Blynk.virtualWrite(BLYNK_VPIN_HUM, humPct);
        Blynk.virtualWrite(BLYNK_VPIN_AQ, aqRaw);
    }
}
