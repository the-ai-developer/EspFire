# EspFire — ESP32 Fire Alarm

Professional ESP32 fire alarm with QC0050 IR flame sensor, dual LED + buzzer, and Blynk IoT alerts. Built for college viva + real home use. Expandable to MQ2 smoke & DHT22.

## Hardware

| Part | Qty |
|------|-----|
| ESP32 DevKit V1 38-pin | 1 |
| QC0050 IR Flame Sensor | 1 |
| Red LED + 220Ω | 1 |
| Green LED + 220Ω | 1 |
| Active Buzzer 3.3V | 1 |
| Jumpers, breadboard | — |

## Wiring

| QC0050 | ESP32 | LED/Buzzer | ESP32 |
|--------|-------|------------|-------|
| VCC | 3.3V | Red LED + → 220Ω → GPIO26 | |
| GND | GND | Red LED - | GND |
| DO | GPIO27 | Green LED + → 220Ω → GPIO25 | |
| AO | GPIO34 | Buzzer + → GPIO33, Buzzer - → GND | |

> Reserved for expansion: MQ2 DO→32 AO→35, DHT22→14. Avoid GPIO 0/2/12/15 (boot).

```
ESP32 3.3V ── QC0050 VCC
GND ──────── QC0050 GND
GPIO27 ←──── QC0050 DO (LOW=fire)
GPIO34 ←──── QC0050 AO (optional analog)
GPIO26 ──220Ω──► Red LED ──► GND
GPIO25 ──220Ω──► Green LED ──► GND
GPIO33 ───────► Buzzer+  Buzzer- ──► GND
```

Adjust QC0050 potentiometer: clockwise = more sensitive. Test with lighter at ~30cm.

## Firmware

- PlatformIO, Arduino framework, Blynk 1.3.2
- Non-blocking (no `delay()`), `millis()` timers
- Sensitive mode: fire if DO LOW for 200ms, clear if HIGH for 1000ms
- Local: Green solid SAFE, Red blink 200ms + Buzzer pulse 300ms on FIRE
- Cloud: Blynk V0 fire 0/1, V1 analog, V2 uptime, `fire_alert` event (1 per 30s)
- Serial 115200 logs state changes + 5s heartbeat

Structure:
```
include/config.h, secrets.h.example
include/sensors/flame_sensor.h
include/indicators/led_buzzer.h
include/core/fire_state.h
include/cloud/blynk_client.h
src/main.cpp + per-module .cpp
```

## Setup

1. **Clone & secrets**
```bash
cp include/secrets.h.example include/secrets.h
# edit WIFI_SSID, WIFI_PASS, BLYNK_AUTH_TOKEN
```

2. **Blynk**
- Create template `EspFire` at blynk.cloud, copy Template ID/Name into `config.h` if custom
- Datastreams: V0 Integer 0/1, V1 Integer 0-4095, V2 Integer seconds
- Event `fire_alert` (critical) + `fire_cleared`
- Copy Auth Token to `secrets.h`

3. **Build & flash**
```bash
pio run -t upload
pio device monitor -b 115200
```

4. **Test**
- Power on → Green ON, serial "Ready"
- Lighter 30cm from QC0050 → Red blink + buzzer <300ms, serial `STATE -> FIRE`, Blynk V0=1 + phone notify
- Remove flame → 1s → Green ON, V0=0

## Troubleshooting

| Symptom | Fix |
|---------|-----|
| Always FIRE in sunlight | Turn QC0050 pot CCW, add `FIRE_DEBOUNCE_MS` 500 if needed |
| Never FIRE | Check DO wiring 27, pot CW, confirm `FLAME_ACTIVE_LOW` true |
| Buzzer silent | Active buzzer needs 3.3V HIGH; test direct 3.3V |
| Blynk offline | Check `secrets.h`, 2.4GHz WiFi, serial `blynk:disconnected` |
| False alerts | Increase `FIRE_DEBOUNCE_MS` or use AO threshold |

## Expansion

Add MQ2 without touching `main.cpp`:
```cpp
// in FireState::update: bool isFire = flameFire || smokeFire;
// Wire MQ2 DO→32, add SmokeSensor reading
```

## License

MIT
