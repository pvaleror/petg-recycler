#ifndef PINS_H
#define PINS_H

// Pin map (ESP32-C3 only) — grouped for easier wiring

// User inputs (side with 4, 3, 2, 1, 0)
const int encButton = 2;                    // Push button (GPIO2)
static int pinA = 4;                        // Encoder A   (GPIO4)
static int pinB = 3;                        // Encoder B   (GPIO3)


// I2C (OLED)
#define I2C_SDA       8                     // I2C SDA (SSD1306)
#define I2C_SCL       9                     // I2C SCL (SSD1306)



// Analog inputs (ADC1)
#define THERM_PIN     1                     // Thermistor analog input (GPIO0)
//#define SPARE_ADC1    1                     // Spare analog input (GPIO1)
#define SPARE_ADC2    5                     // Spare analog input (GPIO5, other side)

// Motor control pins (adjust these to your actual pin numbers)
const int motStepPin = 10;                   // Stepper STEP pin (GPIO10)
const int motDirPin = 6;                     // Stepper DIR pin
const int enablePin = 7;                     // Stepper ENABLE pin
// Set to true if your driver EN is active LOW (A4988/DRV8825). If EN is active HIGH, set to false.
const bool EN_ACTIVE_LOW = true;

#endif // PINS_H

