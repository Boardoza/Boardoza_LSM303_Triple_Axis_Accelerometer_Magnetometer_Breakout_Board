/**
 * Basic_Reading_I2C Example
 * 
 * Demonstrates basic usage of the Boardoza LSM303AGR library using I2C communication.
 * Reads accelerometer, magnetometer, and temperature sensor data and prints to Serial Monitor.
 * 
 * Hardware Setup (I2C Mode):
 * - Connect SDA to Arduino SDA pin (A4 on Uno/Nano, 20 on Mega, 21 on Due)
 * - Connect SCL to Arduino SCL pin (A5 on Uno/Nano, 21 on Mega, 20 on Due)
 * - Connect VDD to 3.3V
 * - Connect GND to GND
 * - Pull-up resistors (4.7kΩ) on SDA and SCL recommended
 * 
 * Default I2C addresses:
 * - Accelerometer: 0x19
 * - Magnetometer: 0x1E
 */

#include <Wire.h>
#include <LSM303AGR.h>

// Create LSM303AGR instance
LSM303AGR sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to open
  
  Serial.println("Boardoza LSM303AGR - Basic Reading (I2C)");
  Serial.println("==========================================");
  
  // Initialize I2C
  Wire.begin();
  
  // Initialize sensor in I2C mode
  if (!sensor.begin()) {
    Serial.println("ERROR: LSM303AGR sensor not detected!");
    Serial.println("Check wiring and I2C address.");
    while (1) delay(10); // Halt
  }
  
  Serial.println("Sensor initialized successfully.");
  
  // Configure accelerometer
  sensor.setAccelRange(2);        // ±2g range
  sensor.setAccelDataRate(100);   // 100Hz output data rate
  sensor.setAccelPowerMode(POWER_MODE_HIGH_RESOLUTION);
  sensor.enableAccelAxes(true, true, true); // Enable all axes
  
  // Configure magnetometer
  sensor.setMagMode(MAG_MODE_CONTINUOUS);    // Continuous measurement mode
  sensor.setMagDataRate(100);                // 100Hz output data rate
  sensor.setMagOffsetCancellation(true);     // Enable hard-iron compensation
  
  // Enable temperature sensor
  sensor.enableTemperature(true);
  
  Serial.println("Starting measurements...");
  Serial.println();
}

void loop() {
  float accelX, accelY, accelZ;
  float magX, magY, magZ;
  float temperature;
  
  // Read accelerometer (m/s²)
  if (sensor.readAccel(accelX, accelY, accelZ)) {
    Serial.print("Accelerometer: ");
    Serial.print("X="); Serial.print(accelX, 2); Serial.print(" m/s²  ");
    Serial.print("Y="); Serial.print(accelY, 2); Serial.print(" m/s²  ");
    Serial.print("Z="); Serial.print(accelZ, 2); Serial.println(" m/s²");
  }
  
  // Read magnetometer (µT - microtesla)
  if (sensor.readMag(magX, magY, magZ)) {
    Serial.print("Magnetometer:  ");
    Serial.print("X="); Serial.print(magX, 1); Serial.print(" µT  ");
    Serial.print("Y="); Serial.print(magY, 1); Serial.print(" µT  ");
    Serial.print("Z="); Serial.print(magZ, 1); Serial.println(" µT");
  }
  
  // Read temperature (°C)
  temperature = sensor.readTemperature();
  Serial.print("Temperature:   ");
  Serial.print(temperature, 1);
  Serial.println(" °C");
  
  Serial.println();
  delay(500); // Read every 500ms for readability
}
