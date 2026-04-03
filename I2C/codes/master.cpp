#include <Arduino.h>
#include <Wire.h>

// Function Prototype (Forward Declaration)
void readSlave(int address); 

void setup() {
  Wire.begin();        // Join I2C bus as Master
  Serial.begin(9600);  // Start serial for Termux communication
  
  Serial.println("--- Resistance Measuring System Ready ---");
  Serial.println("Type '1' to read Slave 1 (Addr 8)");
  Serial.println("Type '2' to read Slave 2 (Addr 9)");
}

void loop() {
  if (Serial.available() > 0) {
    char choice = Serial.read();
    
    if (choice == '1') {
      readSlave(8);
    } else if (choice == '2') {
      readSlave(9);
    }
  }
}

void readSlave(int address) {
  Wire.requestFrom(address, 4); // Request 4 bytes (size of a float)
  
  if (Wire.available() == 4) {
    union {
      float f;
      byte b[4];
    } data;
    
    // Read the 4 bytes from the slave
    for (int i = 0; i < 4; i++) {
      data.b[i] = Wire.read();
    }
    
    // Print the result via Serial to Termux
    Serial.print("Slave "); 
    Serial.print(address - 7); // Math to make Addr 8 = "Slave 1"
    Serial.print(" Unknown Resistance: ");
    
    if (data.f == -1.0) {
      Serial.println("Open Circuit (Nothing connected)");
    } else {
      Serial.print(data.f);
      Serial.println(" Ohms");
    }
  } else {
    Serial.println("Error: Could not read from slave.");
  }
}