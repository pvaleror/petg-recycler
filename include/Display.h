#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Config.h>
#include <Pins.h>
#include <Types.h>

class Display {
private:
    Adafruit_SSD1306 oled;
    int lastTempDisplayed;
    bool initialized;

public:
    Display();
    
    // Initialization
    bool begin();
    
    // Display control
    void clear();
    void update();
    void showInitMessage(const char* message);
    
    // Content display
    void showLabels(MenuState currentMenu);
    void showValues(double currentTemp, double setpoint, int speed, const char* motorDir);
    void showStatus(const char* status);
    
    // Utility
    bool isInitialized() const { return initialized; }

private:
    void setCursorAtLine(int line, bool isValue = false);
};

// Global display instance
extern Display display;

#endif // DISPLAY_H