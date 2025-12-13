#include "Thermistor.h"

// Global thermistor instance
Thermistor thermistor;

Thermistor::Thermistor() {
    currentTemperature = 25.0;
    lastReadTime = 0;
    simulationMode = true; // Start in simulation mode
}

void Thermistor::begin() {
    pinMode(THERM_PIN, INPUT);
    analogReadResolution(12);
    currentTemperature = readTemperatureC();
}

double Thermistor::readTemperatureC() {
    if (simulationMode) {
        currentTemperature = readSimulatedTemperature();
    } else {
        currentTemperature = readRealTemperature();
    }
    return currentTemperature;
}

double Thermistor::readRealTemperature() {
    long sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += analogRead(THERM_PIN);
        delay(2);
    }
    double reading = (double)sum / (double)NUM_SAMPLES;
    if (reading <= 0.5) return -273.15; // avoid div by zero, return min

    double ratio = reading / ADC_MAX;                 // V/VDDA
    if (ratio >= 0.9995) ratio = 0.9995;              // clamp
    if (ratio <= 0.0005) ratio = 0.0005;

    double resistance = SERIES_RESISTOR * (ratio / (1.0 - ratio));

    double steinhart;
    steinhart = resistance / THERMISTOR_NOMINAL;      // (R/R0)
    steinhart = log(steinhart);                       // ln(R/R0)
    steinhart /= B_COEFFICIENT;                       // 1/B * ln(R/R0)
    steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);// + 1/T0
    steinhart = 1.0 / steinhart;                      // Invert
    steinhart -= 273.15;                              // K -> C
    return steinhart;
}

double Thermistor::readSimulatedTemperature() {
    // Simple demo value that changes over time
    return 25.0 + (millis() % 1000) / 100.0;
}

bool Thermistor::checkTemperature(double setpoint, double tolerance) {
    double diff = abs(currentTemperature - setpoint);
    return diff <= tolerance;
}

bool Thermistor::isInRange(double minTempValue, double maxTempValue) const {
    return (currentTemperature >= minTempValue && currentTemperature <= maxTempValue);
}

void Thermistor::update() {
    unsigned long currentTime = millis();
    // Update temperature reading every 100ms
    if (currentTime - lastReadTime >= 100) {
        readTemperatureC();
        lastReadTime = currentTime;
    }
}