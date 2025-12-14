// Test simple del método FabLab para diagnóstico
// Este archivo es solo para testing - no se compila por defecto

#include <Arduino.h>

// Pines del motor (copiados de Pins.h)
const int motStepPin = 10;
const int motDirPin = 6;
const int enablePin = 7;
const bool EN_ACTIVE_LOW = true;

// Variables de control
unsigned long stepCounter = 0;
unsigned long stepInterval = 2000;  // 2ms entre pasos (velocidad media)
bool stepState = false;
bool motorEnabled = false;

void setup() {
    // Configurar pines
    pinMode(motStepPin, OUTPUT);
    pinMode(motDirPin, OUTPUT);
    pinMode(enablePin, OUTPUT);
    
    // Estado inicial
    digitalWrite(motStepPin, LOW);
    digitalWrite(motDirPin, HIGH);  // Dirección forward
    digitalWrite(enablePin, EN_ACTIVE_LOW ? HIGH : LOW);  // Deshabilitado
    
    delay(2000);  // Esperar 2 segundos
    
    // Habilitar motor
    digitalWrite(enablePin, EN_ACTIVE_LOW ? LOW : HIGH);  // Habilitar
    motorEnabled = true;
    
    stepCounter = micros();
}

void loop() {
    if (!motorEnabled) return;
    
    // Método FabLab exacto
    unsigned long currentMicros = micros();
    
    if (currentMicros - stepCounter >= stepInterval) {
        stepCounter = currentMicros;
        
        // Toggle step pin
        stepState = !stepState;
        digitalWrite(motStepPin, stepState ? HIGH : LOW);
    }
}