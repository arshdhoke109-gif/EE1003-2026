#include <Arduino.h>
#include <Wire.h>

// --- CONFIGURATION ---
const float knownR = 10000.0; // 10k Ohm known resistor 
float unknownR = 0.0;
int slaveAddress = 8;        // Set to 8 for Slave 1, and 9 for Slave 2

// Function prototype for I2C event
void requestEvent();

void setup() {
  Wire.begin(slaveAddress);     
  Wire.onRequest(requestEvent); 
}

void loop() {
  // 1. Dummy Read: Discard the first reading to let the ADC stabilize
  analogRead(A0);
  delay(10);

  // 2. Averaging: Take 20 samples to filter out electrical noise
  long sumADC = 0;
  int samples = 20;
  for (int i = 0; i < samples; i++) {
    sumADC += analogRead(A0);
    delay(5);
  }
  float averageADC = (float)sumADC / samples;

  // 3. Resistance Calculation
  // ASSUMPTION: 5V -> Known R (10k) -> A0 -> Unknown R (33k) -> GND
  if (averageADC > 10.0 && averageADC < 1010.0) {
    unknownR = knownR * (averageADC / (1023.0 - averageADC));
  } 
  // Open Circuit logic (Voltage stays high near 1023)
  else if (averageADC >= 1010.0) {
    unknownR = -1.0; 
  } 
  // Short Circuit logic (Voltage drops to 0)
  else {
    unknownR = 0.0;  
  }
  
  delay(100); 
}

void requestEvent() {
  // Union breaks the 4-byte float into bytes for I2C transfer 
  union {
    float f;
    byte b[4];
  } data;
  
  data.f = unknownR;
  Wire.write(data.b, 4); 
}