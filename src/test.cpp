// ESP32-C3 pin mapping aligned with main.cpp (ESP32-only project)
const int stepPin = 7;  // motStepPin
const int dirPin = 6;   // motDirPin
const int enPin = 10;   // enablePin


#include <Arduino.h>                        //Project libraries includes
#include <Wire.h>


void setupT() {
  // Set the pins as outputs
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(enPin, OUTPUT);

  // Enable the motor driver
  digitalWrite(enPin, LOW);

  // Set the initial direction
  digitalWrite(dirPin, HIGH);
}

void loopT() {
  // Rotate the motor 200 steps in one direction
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000); // Adjust delay for speed control
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }

  // Change direction
  digitalWrite(dirPin, LOW);
  delay(1000); // Wait for a second

  // Rotate the motor 200 steps in the other direction
  for (int i = 0; i < 200; i++) {
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(1000);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(1000);
  }

  // Change direction
  digitalWrite(dirPin, HIGH);
  delay(1000); // Wait for a second
}