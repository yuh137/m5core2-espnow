#include "screen.h"

void setupScreen()
{
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(ORANGE);
    M5.Lcd.drawString("Smart Home", 10, 0);
    M5.Lcd.drawFastHLine(10, 20, 300, WHITE);

    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.drawString("Temperature(C):", 10, 30);

    M5.Lcd.setTextColor(CYAN);
    M5.Lcd.drawString("Humidity(%):", 10, 70);

    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.drawString("Light: ", 10, 110);

    M5.Lcd.setTextColor(CYAN);
    M5.Lcd.drawString("Fan:", 10, 150);
}

void updateSensorField(float temp, float humid)
{
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.fillRect(220, 30, 70, 20, BLACK);
    M5.Lcd.drawFloat(temp, 2, 220, 30);
    log_i("temperature: %.2f", temp);

    M5.Lcd.setTextColor(CYAN);
    M5.Lcd.fillRect(220, 70, 70, 20, BLACK);
    M5.Lcd.drawFloat(humid, 2, 220, 70);
    log_i("humidity: %.2f", humid);
}

void updateDevicesField(int fan, int light)
{
    M5.Lcd.setTextSize(2);
    M5.Lcd.setTextColor(YELLOW);
    M5.Lcd.fillRect(220, 110, 70, 20, BLACK);
    M5.Lcd.drawString(light == 1 ? "ON" : "OFF", 220, 110);

    M5.Lcd.setTextColor(CYAN);
    M5.Lcd.fillRect(220, 150, 70, 20, BLACK);
    M5.Lcd.drawString(fan == 1 ? "ON" : "OFF", 220, 150);
}