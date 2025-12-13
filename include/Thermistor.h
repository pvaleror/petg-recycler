#ifndef THERMISTOR_H
#define THERMISTOR_H

#include <Arduino.h>
#include <Config.h>
#include <Pins.h>

class Thermistor {
private:
    double currentTemperature;
    unsigned long lastReadTime;
    bool simulationMode;

public:
    Thermistor();
    
    // Initialization
    void begin();
    
    // Temperature reading
    double readTemperatureC();
    double getCurrentTemperature() const { return currentTemperature; }
    
    // Temperature checking
    bool checkTemperature(double setpoint, double tolerance = tempTolerance);
    bool isInRange(double minTempValue, double maxTempValue) const;
    
    // Simulation mode (for testing)
    void setSimulationMode(bool enabled) { simulationMode = enabled; }
    bool isSimulationMode() const { return simulationMode; }
    
    // Update function (call periodically)
    void update();

private:
    double readRealTemperature();
    double readSimulatedTemperature();
};

// Global thermistor instance
extern Thermistor thermistor;

#endif // THERMISTOR_H