#include "Thermistor.h"
#include <math.h>

#ifndef PI
#define PI 3.14159265359
#endif

// Global thermistor instance
Thermistor thermistor;

Thermistor::Thermistor() {
    currentTemperature = 25.0;
    lastReadTime = 0;
}

void Thermistor::begin() {
    pinMode(THERM_PIN, INPUT);
    //analogSetAttenuation(ADC_11db);  // Rango 0-3.3V completo
    analogSetPinAttenuation(THERM_PIN, ADC_11db);  // Específico para GPIO0
    analogReadResolution(12);
    currentTemperature = readTemperatureC();
}

double Thermistor::readTemperatureC() {
    long sum = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        sum += analogRead(THERM_PIN);
        delay(2);
    }

    
    double reading = (double)sum / (double)NUM_SAMPLES;
    
    // Ratio ADC (0.0 a 1.0)
    double ratio = reading / ADC_MAX;
    
    // Clamp para evitar divisiones por cero
    if (ratio >= 0.99) ratio = 0.99;
    if (ratio <= 0.01) ratio = 0.01;

    // Cálculo correcto para el circuito actual:
    // GND ──[Termistor]──┬──[51kΩ]── 3.3V
    //                   │
    //                GPIO0
    // Cuando ratio es bajo (voltaje bajo), termistor tiene baja resistencia (alta temperatura)
    // Cuando ratio es alto (voltaje alto), termistor tiene alta resistencia (baja temperatura)
    double resistance = SERIES_RESISTOR * ((1.0 - ratio) / ratio);

    double steinhart;
    steinhart = resistance / THERMISTOR_NOMINAL;      // (R/R0)
    steinhart = log(steinhart);                       // ln(R/R0)
    steinhart /= B_COEFFICIENT;                       // 1/B * ln(R/R0)
    steinhart += 1.0 / (TEMPERATURE_NOMINAL + 273.15);// + 1/T0
    steinhart = 1.0 / steinhart;                      // Invert
    steinhart -= 273.15;                              // K -> C
    
    // Calibración basada en mediciones reales:
    // 40°C real → 36°C medido
    // 94°C real → 90°C medido
    // 100°C real → 107°C medido  
    // 118°C real → 107°C medido
    // 175°C real → 107°C medido (saturación)
    // 190°C real → 107°C medido (saturación)
    
    double calibratedTemp = steinhart;
    
    if (steinhart <= 50) {
        // Rango bajo: 36°C medido → 40°C real
        calibratedTemp = steinhart * 1.111; // Factor de corrección 40/36
    } else if (steinhart <= 90) {
        // Rango medio: interpolación lineal entre 36→40 y 90→94
        calibratedTemp = 40 + (steinhart - 36) * (94 - 40) / (90 - 36);
    } else if (steinhart <= 107) {
        // Rango de saturación: el termistor se satura en 107°C
        // Necesitamos extrapolar basándose en el punto donde empieza la saturación
        // 100°C real → 107°C medido, entonces extrapolamos linealmente
        double saturationPoint = 107.0;
        double realTempAtSaturation = 100.0;
        
        // Extrapolación agresiva para temperaturas altas
        calibratedTemp = realTempAtSaturation + (steinhart - saturationPoint) * 10.0;
        
        // Si está exactamente en 107°C, asumimos que puede ser cualquier temperatura alta
        if (abs(steinhart - 107.0) < 0.5) {
            // Temperatura indeterminada en rango alto, usar valor conservador
            calibratedTemp = 120.0; // Valor por defecto para saturación
        }
    } else {
        // Por encima de la saturación (no debería ocurrir, pero por seguridad)
        calibratedTemp = 200.0; // Valor alto por defecto
    }
    
    // Validación de rango razonable
    if (calibratedTemp < -50.0 || calibratedTemp > 500.0) {
        return -273.15; // Error: temperatura fuera de rango
    }
    
    return calibratedTemp;
}

double Thermistor::readSimulatedTemperature() {
    // Simulación más realista para pruebas
    // Temperatura que oscila lentamente entre 220-240°C
    double baseTemp = 230.0;
    double variation = 10.0 * sin((millis() / 5000.0) * 2.0 * PI);
    return baseTemp + variation;
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
        currentTemperature = readTemperatureC();
        lastReadTime = currentTime;
    }
}