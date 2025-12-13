#include "Display.h"

// Global display instance
Display display;

Display::Display() : oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1) {
    lastTempDisplayed = -10000;
    initialized = false;
}

bool Display::begin() {
    // Initialize I2C
    Wire.begin(I2C_SDA, I2C_SCL);
    delay(100);
    
    // Initialize OLED display
    if(!oled.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        return false;
    }
    
    oled.setTextSize(1);
    oled.setTextColor(SSD1306_WHITE);
    
    initialized = true;
    return true;
}

void Display::clear() {
    if (!initialized) return;
    oled.clearDisplay();
}

void Display::update() {
    if (!initialized) return;
    oled.display();
}

void Display::showInitMessage(const char* message) {
    if (!initialized) return;
    
    clear();
    oled.setCursor(0, 0);
    oled.println(message);
    update();
}

void Display::setCursorAtLine(int line, bool isValue) {
    int x = isValue ? LINE_X2 : LINE_X1;
    int y;
    
    switch(line) {
        case 1: y = LINE_Y1; break;
        case 2: y = LINE_Y2; break;
        case 3: y = LINE_Y3; break;
        case 4: y = LINE_Y4; break;
        default: y = 0; break;
    }
    
    oled.setCursor(x, y);
}

void Display::showLabels(MenuState currentMenu) {
    if (!initialized) return;
    
    // Header
    setCursorAtLine(1);
    oled.print(F(" Current Temp:"));
    
    // Menu items with selection indicator
    setCursorAtLine(2);
    oled.print(currentMenu == MENU_TEMPERATURE ? ">Set Temp:" : " Set Temp:");
    
    setCursorAtLine(3);
    oled.print(currentMenu == MENU_SPEED ? ">Extrude Speed:" : " Extrude Speed:");
    
    setCursorAtLine(4);
    oled.print(currentMenu == MENU_EXTRUDE ? ">Extrude:" : " Extrude:");
}

void Display::showValues(double currentTemp, double setpoint, int speed, const char* motorDir) {
    if (!initialized) return;
    
    // Current temperature
    setCursorAtLine(1, true);
    oled.print((int)currentTemp);
    oled.print(F("C"));
    
    // Setpoint temperature
    setCursorAtLine(2, true);
    oled.print((int)setpoint);
    oled.print(F("C"));
    
    // Speed
    setCursorAtLine(3, true);
    oled.print(speed);
    
    // Motor direction
    setCursorAtLine(4, true);
    oled.print(motorDir);
}

void Display::showStatus(const char* status) {
    if (!initialized) return;
    
    oled.setCursor(STATUS_X, STATUS_Y);
    oled.print(status);
}