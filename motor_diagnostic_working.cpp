// MOTOR DIAGNOSTIC PROGRAM
#include <Arduino.h>

// Pin definitions (from your Pins.h)
const int motStepPin = 10;   // GPIO10
const int motDirPin = 6;     // GPIO6  
const int enablePin = 7;     // GPIO7
const bool EN_ACTIVE_LOW = true;

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== MOTOR DIAGNOSTIC PROGRAM ===");
    Serial.println("Testing motor pins and basic functionality");
    
    // Initialize pins
    pinMode(motStepPin, OUTPUT);
    pinMode(motDirPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    
    // Set initial states
    digitalWrite(motStepPin, LOW);
    digitalWrite(motDirPin, HIGH);  // Forward direction
    
    // Enable motor (EN is active LOW for A4988/DRV8825)
    digitalWrite(enablePin, LOW);  // Enable motor
    
    Serial.println("\nPin configuration:");
    Serial.printf("Step Pin (GPIO%d): %s\n", motStepPin, digitalRead(motStepPin) ? "HIGH" : "LOW");
    Serial.printf("Dir Pin (GPIO%d): %s\n", motDirPin, digitalRead(motDirPin) ? "HIGH" : "LOW");
    Serial.printf("Enable Pin (GPIO%d): %s (Motor ENABLED)\n", enablePin, digitalRead(enablePin) ? "HIGH" : "LOW");
    Serial.printf("EN_ACTIVE_LOW: %s\n", EN_ACTIVE_LOW ? "true" : "false");
    
    Serial.println("\nChecklist for troubleshooting:");
    Serial.println("1. VMOT connected to +12V?");
    Serial.println("2. GND connected to common ground?");
    Serial.println("3. Motor coils connected to A1,A2,B1,B2?");
    Serial.println("4. MS1,MS2,MS3 left floating (unconnected)?");
    Serial.println("5. Driver power LED on?");
    
    Serial.println("\nMotor should start stepping in 3 seconds...");
    Serial.println("Commands: 'd'=toggle direction, 'e'=toggle enable, 's'=show status, 'f'=fast, 'l'=slow");
    delay(3000);
}

void loop() {
    static unsigned long lastStep = 0;
    static bool stepState = false;
    static unsigned long stepInterval = 2000; // Start slow (2ms)
    
    if (micros() - lastStep >= stepInterval) {
        stepState = !stepState;
        digitalWrite(motStepPin, stepState ? HIGH : LOW);
        lastStep = micros();
        
        // Print status every 200 steps
        static int stepCount = 0;
        if (stepState) { // Count on rising edge
            stepCount++;
            if (stepCount % 200 == 0) {
                Serial.printf("Steps: %d (Interval: %lu us)\n", stepCount, stepInterval);
            }
        }
    }
    
    // Check for serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'd':
                digitalWrite(motDirPin, !digitalRead(motDirPin));
                Serial.printf("Direction: %s\n", digitalRead(motDirPin) ? "FORWARD" : "REVERSE");
                delay(100); // Direction settling time
                break;
            case 'e':
                digitalWrite(enablePin, !digitalRead(enablePin));
                Serial.printf("Enable: %s (Motor %s)\n", 
                    digitalRead(enablePin) ? "HIGH" : "LOW",
                    digitalRead(enablePin) == (EN_ACTIVE_LOW ? LOW : HIGH) ? "ENABLED" : "DISABLED");
                break;
            case 'f':
                stepInterval = 500; // Fast (500us = 2000 steps/sec)
                Serial.println("Speed: FAST");
                break;
            case 'l':
                stepInterval = 5000; // Slow (5ms = 200 steps/sec)
                Serial.println("Speed: SLOW");
                break;
            case 's':
                Serial.println("\n=== CURRENT STATUS ===");
                Serial.printf("Step Pin (GPIO%d): %s\n", motStepPin, digitalRead(motStepPin) ? "HIGH" : "LOW");
                Serial.printf("Dir Pin (GPIO%d): %s\n", motDirPin, digitalRead(motDirPin) ? "HIGH" : "LOW");
                Serial.printf("Enable Pin (GPIO%d): %s\n", enablePin, digitalRead(enablePin) ? "HIGH" : "LOW");
                Serial.printf("Motor State: %s\n", digitalRead(enablePin) == (EN_ACTIVE_LOW ? LOW : HIGH) ? "ENABLED" : "DISABLED");
                Serial.printf("Step Interval: %lu microseconds\n", stepInterval);
                Serial.println("======================\n");
                break;
            case 'h':
                Serial.println("\n=== HELP ===");
                Serial.println("d - Toggle direction");
                Serial.println("e - Toggle enable/disable");
                Serial.println("f - Fast speed");
                Serial.println("l - Slow speed");
                Serial.println("s - Show status");
                Serial.println("h - This help");
                Serial.println("=============\n");
                break;
        }
    }
}