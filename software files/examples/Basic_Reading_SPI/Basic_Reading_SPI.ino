/**
 * Basic_Reading_SPI Example
 * 
 * Demonstrates basic usage of the Boardoza LSM303AGR library using SPI communication.
 * 
 * ⚡ COMPETITIVE ADVANTAGE: Unlike other LSM303AGR libraries, Boardoza supports BOTH
 * I2C and SPI communication! SPI provides up to 25x faster data transfer (10 MHz vs
 * 400 kHz I2C) for high-speed data logging and real-time applications.
 * 
 * Hardware Setup (SPI Mode):
 * - Connect MOSI to Arduino MOSI (pin 11 on Uno/Nano, 51 on Mega)
 * - Connect MISO to Arduino MISO (pin 12 on Uno/Nano, 50 on Mega)
 * - Connect SCK to Arduino SCK (pin 13 on Uno/Nano, 52 on Mega)
 * - Connect CS_ACCEL to digital pin 10 (configurable)
 * - Connect CS_MAG to digital pin 9 (configurable)
 * - Connect VDD to 3.3V
 * - Connect GND to GND
 * 
 * Note: LSM303AGR requires separate chip select pins for accelerometer and magnetometer.
 */

#include <SPI.h>
#include <LSM303AGR.h>

// Define chip select pins
#define CS_ACCEL 10
#define CS_MAG   9

// Create LSM303AGR instance
LSM303AGR sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for Serial Monitor to open
  
  Serial.println("Boardoza LSM303AGR - Basic Reading (SPI)");
  Serial.println("==========================================");
  Serial.println("🚀 SPI Mode: Up to 25x faster than I2C!");
  Serial.println();
  
  // Initialize SPI
  SPI.begin();
  
  // Initialize sensor in SPI mode
  if (!sensor.beginSPI(CS_ACCEL, CS_MAG)) {
    Serial.println("ERROR: LSM303AGR sensor not detected!");
    Serial.println("Check wiring and chip select pins.");
    while (1) delay(10); // Halt
  }
  
  Serial.println("Sensor initialized successfully in SPI mode.");
  
  // Optional: Set SPI clock speed for high-speed logging (default is 1 MHz)
  sensor.setSPIClockSpeed(10000000); // 10 MHz for maximum throughput
  Serial.println("SPI clock: 10 MHz (maximum speed)");
  
  // Configure accelerometer
  sensor.setAccelRange(2);        // ±2g range
  sensor.setAccelDataRate(400);   // 400Hz output data rate (faster sampling!)
  sensor.setAccelPowerMode(POWER_MODE_HIGH_RESOLUTION);
  sensor.enableAccelAxes(true, true, true); // Enable all axes
  
  // Configure magnetometer
  sensor.setMagMode(MAG_MODE_CONTINUOUS);    // Continuous measurement mode
  sensor.setMagDataRate(100);                // 100Hz output data rate
  sensor.setMagOffsetCancellation(true);     // Enable hard-iron compensation
  
  // Enable temperature sensor
  sensor.enableTemperature(true);
  
  Serial.println("Starting high-speed measurements...");
  Serial.println();
}

void loop() {
  float accelX, accelY, accelZ;
  float magX, magY, magZ;
  float temperature;
  
  // Check if new accelerometer data is available
  if (sensor.isAccelDataReady()) {
    // Read accelerometer (m/s²)
    sensor.readAccel(accelX, accelY, accelZ);
    Serial.print("Accel: ");
    Serial.print("X="); Serial.print(accelX, 2); Serial.print(" ");
    Serial.print("Y="); Serial.print(accelY, 2); Serial.print(" ");
    Serial.print("Z="); Serial.print(accelZ, 2); Serial.print(" m/s²  ");
  }
  
  // Check if new magnetometer data is available
  if (sensor.isMagDataReady()) {
    // Read magnetometer (µT)
    sensor.readMag(magX, magY, magZ);
    Serial.print("Mag: ");
    Serial.print("X="); Serial.print(magX, 1); Serial.print(" ");
    Serial.print("Y="); Serial.print(magY, 1); Serial.print(" ");
    Serial.print("Z="); Serial.print(magZ, 1); Serial.print(" µT  ");
  }
  
  // Read temperature (°C)
  temperature = sensor.readTemperature();
  Serial.print("Temp: "); Serial.print(temperature, 1); Serial.println(" °C");
  
  // Faster sampling rate enabled by SPI!
  // I2C mode typically uses delay(500), but SPI can handle delay(100) or faster
  delay(100); // 10Hz display update (sensor reads at 400Hz)
}
