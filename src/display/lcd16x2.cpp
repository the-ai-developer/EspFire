#include "display/lcd16x2.h"
#include "config.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

static LiquidCrystal_I2C lcd(LCD_I2C_ADDR, LCD_COLS, LCD_ROWS);

void LCD16x2::begin() {
    Wire.begin(PIN_LCD_SDA, PIN_LCD_SCL);
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("EnvMonitor boot");
    lcd.setCursor(0, 1);
    lcd.print("Warming MQ2...");
    ready_ = true;
}

void LCD16x2::update(const DHTReading& dht, int aqRaw, bool abnormal, bool wifiOk) {
    if (!ready_) return;
    unsigned long now = millis();
    if (now - lastRefresh_ < LCD_REFRESH_MS) return;
    lastRefresh_ = now;

    char line0[17];
    char line1[17];
    if (dht.valid) {
        snprintf(line0, sizeof(line0), "T:%.1fC H:%.0f%%", dht.tempC, dht.humPct);
    } else {
        snprintf(line0, sizeof(line0), "T:-- H:--");
    }
    if (abnormal) {
        snprintf(line1, sizeof(line1), "AQ:%-4d ALERT!", aqRaw);
    } else {
        snprintf(line1, sizeof(line1), "AQ:%-4d %s", aqRaw, wifiOk ? "OK" : "..");
    }

    lcd.setCursor(0, 0);
    lcd.print(line0);
    for (int i = strlen(line0); i < LCD_COLS; i++) lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print(line1);
    for (int i = strlen(line1); i < LCD_COLS; i++) lcd.print(" ");
}
