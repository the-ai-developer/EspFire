#!/usr/bin/env python3
"""Seed realistic env telemetry to Blynk so the dashboard shows live data.

Usage:
    python3 tools/seed_dashboard.py [cycles]
    - cycles: how many 5s pushes (default: endless until Ctrl+C)

Reads token from include/secrets.h (BLYNK_AUTH_TOKEN) so no secrets live here.
Requires datastreams V0/V1/V2/V3/V4/V5 + events fire_alert/fire_cleared
in the Blynk template (see blynk-template.json).
"""
import math
import random
import re
import sys
import time
import urllib.request

BASE = "https://blynk.cloud/external/api"
REPO = "/home/princetheprogrammerbtw/EspFire"


def load_token():
    with open(f"{REPO}/include/secrets.h") as f:
        src = f.read()
    m = re.search(r'#define\s+BLYNK_AUTH_TOKEN\s+"([^"]+)"', src)
    if not m:
        raise SystemExit("BLYNK_AUTH_TOKEN not found in include/secrets.h")
    return m.group(1)


def api(path):
    try:
        with urllib.request.urlopen(path, timeout=10) as r:
            return r.read().decode().strip()
    except Exception as e:
        return f"ERR:{e}"


def main():
    token = load_token()
    cycles = int(sys.argv[1]) if len(sys.argv) > 1 else -1
    print(f"[seed] pushing to Blynk every 5s (cycles={'inf' if cycles < 0 else cycles})")
    t0 = time.time()
    i = 0
    alert_on = False
    while cycles < 0 or i < cycles:
        t = time.time() - t0
        temp = 31.5 + 1.2 * math.sin(t / 300) + random.uniform(-0.2, 0.2)
        hum = 70.0 + 6.0 * math.sin(t / 420 + 1.0) + random.uniform(-0.8, 0.8)
        # smoke event every ~12th cycle: AQ spikes over the 1800 alert line
        smoking = (i % 12) >= 10
        aq = random.randint(1950, 2400) if smoking else random.randint(320, 620)
        abnormal = temp >= 40.0 or hum >= 80.0 or aq >= 1800
        uptime = int(t)

        api(f"{BASE}/update?token={token}&V0={1 if abnormal else 0}")
        api(f"{BASE}/update?token={token}&V1={aq}")
        api(f"{BASE}/update?token={token}&V2={uptime}")
        api(f"{BASE}/update?token={token}&V3={temp:.1f}")
        api(f"{BASE}/update?token={token}&V4={hum:.1f}")
        api(f"{BASE}/update?token={token}&V5={aq}")

        if abnormal and not alert_on:
            api(f"{BASE}/logEvent?token={token}&code=fire_alert&description=Seeded+demo+alert")
            alert_on = True
        elif not abnormal and alert_on:
            api(f"{BASE}/logEvent?token={token}&code=fire_cleared&description=Seeded+demo+cleared")
            alert_on = False

        print(f"[{i}] T:{temp:.1f}C H:{hum:.1f}% AQ:{aq} -> {'ABNORMAL' if abnormal else 'normal'}")
        i += 1
        time.sleep(5)


if __name__ == "__main__":
    main()
