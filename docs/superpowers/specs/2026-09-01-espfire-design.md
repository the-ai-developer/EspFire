# EspFire — Design Spec (Pro Viva Build)

**Date:** 2026-09-01
**Status:** Approved — implementing
**Goal:** Mix (College Demo + Home Deployable) with future expandability
**Visual companion:** http://localhost:52231

## 1. Constraints & Kit
- ESP32 DevKit V1 38-pin, QC0050 IR Flame Sensor (DO+AO), Red LED, Green LED, Active Buzzer (3.3V), jumpers, home WiFi
- PlatformIO + Arduino framework
- Cloud: Blynk IoT 2.0 (V0 fire, V1 analog, V2 uptime, event `fire_alert`)
- Sensitive trigger for demo: 200ms debounce, clear after 1s high, Blynk rate limit 30s

## 2. Wiring (viva-safe pins, no boot-strap)
| Component | ESP32 | Note |
|-----------|-------|------|
| QC0050 VCC | 3.3V | 3.3V logic safe |
| QC0050 GND | GND |  |
| QC0050 DO | GPIO27 | LOW = fire (invert flag configurable) |
| QC0050 AO | GPIO34 (optional) | input-only, read via analogRead |
| Red LED | GPIO26 → 220Ω → GND | blink 200ms on fire |
| Green LED | GPIO25 → 220Ω → GND | solid on safe |
| Buzzer | GPIO33 | HIGH = on (active buzzer), pulse 300ms |
| Future MQ2 DO | GPIO32 | reserved |
| Future MQ2 AO | GPIO35 | reserved |
| Future DHT22 | GPIO14 | reserved |

## 3. Behavior
- **SAFE:** Green ON, Red OFF, Buzzer OFF
- **FIRE:** Red blink 200ms, Buzzer pulse 300ms ON/OFF, Blynk V0=1 + logEvent (max 1/30s)
- Detection: `isFireRaw = digitalRead(DO)==LOW`; confirmed if raw held 200ms; cleared if HIGH 1000ms
- Non-blocking: all timing via `millis()`, no `delay()`
- Serial: 115200 baud, state transitions logged with `millis()` and AO value

## 4. Architecture
```
include/
  config.h          // pins, thresholds, Blynk V-pins, WiFi placeholder
  secrets.h.example // template, secrets.h gitignored
  sensors/flame_sensor.h
  indicators/led_buzzer.h
  cloud/blynk_client.h
  core/fire_state.h
src/
  main.cpp          // setup + loop orchestration only
  sensors/flame_sensor.cpp
  indicators/led_buzzer.cpp
  cloud/blynk_client.cpp
  core/fire_state.cpp
platformio.ini
```

- `FlameSensor::isFire()` debounced
- `FireState` FSM: SAFE → FIRE → SAFE with debounce/clear timers
- `LedBuzzer::update(state)` handles blink/pulse without blocking
- `BlynkClient::update(state)` handles connect, virtualWrite rate-limit, `logEvent("fire_alert")`
- Expandable: `FireState` consumes `bool isFlameFire` now; later `isFire = isFlameFire || isSmokeFire || isOverTemp`

## 5. Blynk Setup
- Template: EspFire, Datastreams V0 Integer 0/1, V1 Integer 0-4095, V2 Integer uptime sec
- Event: `fire_alert` (critical) → phone notification
- Dashboard: LED V0, Gauge V1, Value V2, Notification widget
- Auth token via `secrets.h` (`BLYNK_AUTH_TOKEN`)

## 6. Professional Touches (Pro tier)
- README with wiring table, photo placeholder, Blynk import steps, troubleshooting (sunlight false, inverted DO, WiFi fail)
- `.gitignore` for secrets.h, `.pio/`, `.superpowers/`
- No slop: pin defines in config.h, no magic numbers, no String, functions <30 lines
- Build verified via `pio run`

## 7. Future Expansion (no rework)
- Add MQ2: include smoke_sensor.h, OR into FireState
- Add DHT22: temp threshold >50C as fire corroboration
- Add Telegram fallback: implement `cloud/telegram_client.h` same interface as BlynkClient
- Reserved pins already documented

## 8. Verification
- `pio run` must pass before commit
- Manual test: lighter at 30cm → red+buzzer <300ms, Blynk notify within 2s, serial log, clear in 1s after flame removed
