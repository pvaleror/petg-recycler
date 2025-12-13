// Test program for motor debugging
// Uncomment this and comment main.cpp to test motor directly

/*
#include <Arduino.h>
#include <Pins.h>
#include <Config.h>

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("=== MOTOR TEST PROGRAM ===");
    Serial.println("Testing motor pins and basic functionality");
    
    // Initialize pins
    pinMode(motStepPin, OUTPUT);
    pinMode(motDirPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    
    // Set initial states
    digitalWrite(motStepPin, LOW);
    digitalWrite(motDirPin, HIGH);  // Forward direction
    
    // Enable motor (considering EN_ACTIVE_LOW)
    digitalWrite(enablePin, EN_ACTIVE_LOW ? LOW : HIGH);
    
    Serial.println("Pin configuration:");
    Serial.printf("Step Pin (GPIO%d): %s\n", motStepPin, digitalRead(motStepPin) ? "HIGH" : "LOW");
    Serial.printf("Dir Pin (GPIO%d): %s\n", motDirPin, digitalRead(motDirPin) ? "HIGH" : "LOW");
    Serial.printf("Enable Pin (GPIO%d): %s\n", enablePin, digitalRead(enablePin) ? "HIGH" : "LOW");
    Serial.printf("EN_ACTIVE_LOW: %s\n", EN_ACTIVE_LOW ? "true" : "false");
    
    Serial.println("\nStarting motor test in 3 seconds...");
    delay(3000);
}

void loop() {
    static unsigned long lastStep = 0;
    static bool stepState = false;
    const unsigned long stepInterval = 1000; // 1ms = 1000 steps/sec
    
    if (micros() - lastStep >= stepInterval) {
        stepState = !stepState;
        digitalWrite(motStepPin, stepState ? HIGH : LOW);
        lastStep = micros();
        
        // Print status every 1000 steps
        static int stepCount = 0;
        if (stepState) { // Count on rising edge
            stepCount++;
            if (stepCount % 1000 == 0) {
                Serial.printf("Steps: %d\n", stepCount);
            }
        }
    }
    
    // Check for serial commands
    if (Serial.available()) {
        char cmd = Serial.read();
        switch (cmd) {
            case 'd':
                // Toggle direction
                digitalWrite(motDirPin, !digitalRead(motDirPin));
                Serial.printf("Direction: %s\n", digitalRead(motDirPin) ? "HIGH" : "LOW");
                break;
            case 'e':
                // Toggle enable
                digitalWrite(enablePin, !digitalRead(enablePin));
                Serial.printf("Enable: %s\n", digitalRead(enablePin) ? "HIGH" : "LOW");
                break;
            case 's':
                // Show status
                Serial.println("Current pin states:");
                Serial.printf("Step: %s, Dir: %s, Enable: %s\n", 
                    digitalRead(motStepPin) ? "HIGH" : "LOW",
                    digitalRead(motDirPin) ? "HIGH" : "LOW",
                    digitalRead(enablePin) ? "HIGH" : "LOW");
                break;
        }
    }
}
*/