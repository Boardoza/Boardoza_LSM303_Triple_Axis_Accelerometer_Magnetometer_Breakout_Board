/**
 * Self_Test Example
 * 
 * Demonstrates the built-in self-test functionality of the LSM303AGR sensor.
 * Self-tests verify that the MEMS sensing elements and signal paths are functioning
 * correctly by applying known stimuli and checking the response.
 * 
 * This example works with BOTH I2C and SPI modes - uncomment your preferred mode below.
 * 
 * When to run self-tests:
 * - During initial hardware validation
 * - After mechanical shock or drop
 * - Periodically in mission-critical applications
 * - When debugging unexpected sensor readings
 * 
 * Self-test procedure:
 * 1. Captures baseline sensor readings
 * 2. Applies electrostatic force (accelerometer) or magnetic field (magnetometer)
 * 3. Captures stimulated readings
 * 4. Compares difference to expected ranges
 * 5. Restores original configuration
 * 
 * Hardware Setup:
 * - See Basic_Reading_I2C or Basic_Reading_SPI examples for wiring
 */

#include <LSM303AGR.h>

// ===== CONFIGURATION: Choose communication mode =====
#define USE_I2C  // Comment this line and uncomment USE_SPI for SPI mode
// #define USE_SPI

#ifdef USE_SPI
  #include <SPI.h>
  #define CS_ACCEL 10
  #define CS_MAG   9
#else
  #include <Wire.h>
#endif

LSM303AGR sensor;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("Boardoza LSM303AGR - Self-Test Verification");
  Serial.println("=============================================");
  Serial.println();
  
#ifdef USE_SPI
  SPI.begin();
  if (!sensor.beginSPI(CS_ACCEL, CS_MAG)) {
#else
  Wire.begin();
  if (!sensor.begin()) {
#endif
    Serial.println("❌ ERROR: Sensor not detected!");
    Serial.println("   Check wiring and connections.");
    while (1) delay(10);
  }
  
#ifdef USE_SPI
  Serial.println("✓ Sensor detected (SPI mode)");
#else
  Serial.println("✓ Sensor detected (I2C mode)");
#endif
  Serial.println();
  
  // Configure sensor for self-test
  Serial.println("Configuring sensor for self-test...");
  sensor.setAccelRange(2);
  sensor.setAccelDataRate(50);
  sensor.setAccelPowerMode(POWER_MODE_HIGH_RESOLUTION);
  sensor.enableAccelAxes(true, true, true);
  
  sensor.setMagMode(MAG_MODE_CONTINUOUS);
  sensor.setMagDataRate(50);
  
  delay(100); // Stabilization time
  Serial.println("Configuration complete.");
  Serial.println();
  
  // ===== Accelerometer Self-Test =====
  Serial.println("===========================================");
  Serial.println(" ACCELEROMETER SELF-TEST");
  Serial.println("===========================================");
  Serial.println("Running accelerometer self-test...");
  Serial.println("(This applies electrostatic force to verify MEMS operation)");
  Serial.println();
  
  bool accelTestPass = sensor.runAccelSelfTest();
  
  if (accelTestPass) {
    Serial.println("✅ ACCELEROMETER SELF-TEST: PASS");
    Serial.println("   Sensor response within expected range (60-1700 mg)");
    Serial.println("   MEMS sensing element is functioning correctly");
  } else {
    Serial.println("❌ ACCELEROMETER SELF-TEST: FAIL");
    Serial.println("   Sensor response outside expected range");
    Serial.println("   Possible issues:");
    Serial.println("   - Hardware damage or defect");
    Serial.println("   - Poor electrical connection");
    Serial.println("   - MEMS element malfunction");
    Serial.println();
    Serial.println("   Recommended actions:");
    Serial.println("   - Check all wiring connections");
    Serial.println("   - Try a different sensor");
    Serial.println("   - Contact technical support");
  }
  
  Serial.println();
  delay(1000);
  
  // ===== Magnetometer Self-Test =====
  Serial.println("===========================================");
  Serial.println(" MAGNETOMETER SELF-TEST");
  Serial.println("===========================================");
  Serial.println("Running magnetometer self-test...");
  Serial.println("(This applies internal magnetic field to verify sensor operation)");
  Serial.println();
  
  bool magTestPass = sensor.runMagSelfTest();
  
  if (magTestPass) {
    Serial.println("✅ MAGNETOMETER SELF-TEST: PASS");
    Serial.println("   Sensor response within expected range (>1.5 mT)");
    Serial.println("   Magnetic sensing element is functioning correctly");
  } else {
    Serial.println("❌ MAGNETOMETER SELF-TEST: FAIL");
    Serial.println("   Sensor response outside expected range");
    Serial.println("   Possible issues:");
    Serial.println("   - Hardware damage or defect");
    Serial.println("   - Poor electrical connection");
    Serial.println("   - Magnetic sensor malfunction");
    Serial.println("   - Strong external magnetic interference");
    Serial.println();
    Serial.println("   Recommended actions:");
    Serial.println("   - Move away from strong magnetic sources");
    Serial.println("   - Check all wiring connections");
    Serial.println("   - Try a different sensor");
    Serial.println("   - Contact technical support");
  }
  
  Serial.println();
  delay(1000);
  
  // ===== Final Summary =====
  Serial.println("===========================================");
  Serial.println(" SELF-TEST SUMMARY");
  Serial.println("===========================================");
  Serial.print("Accelerometer: ");
  Serial.println(accelTestPass ? "✅ PASS" : "❌ FAIL");
  Serial.print("Magnetometer:  ");
  Serial.println(magTestPass ? "✅ PASS" : "❌ FAIL");
  Serial.println();
  
  if (accelTestPass && magTestPass) {
    Serial.println("🎉 ALL TESTS PASSED!");
    Serial.println("   Sensor is functioning correctly and ready for use.");
    Serial.println();
    Serial.println("Continuing to normal operation...");
    Serial.println("(Displaying live sensor readings)");
    Serial.println();
    
    // Reconfigure for normal operation
    sensor.setAccelDataRate(100);
    sensor.setMagDataRate(100);
    sensor.enableTemperature(true);
  } else {
    Serial.println("⚠️  ONE OR MORE TESTS FAILED");
    Serial.println("   Sensor may not function correctly.");
    Serial.println("   Review failure messages above for troubleshooting.");
    Serial.println();
    Serial.println("Halting. Press reset to retest.");
    while (1) delay(10); // Halt on failure
  }
}

void loop() {
  float accelX, accelY, accelZ;
  float magX, magY, magZ;
  float temperature;
  
  // Read sensor data
  if (sensor.isAccelDataReady()) {
    sensor.readAccel(accelX, accelY, accelZ);
    Serial.print("Accel: ");
    Serial.print("X="); Serial.print(accelX, 2); Serial.print(" ");
    Serial.print("Y="); Serial.print(accelY, 2); Serial.print(" ");
    Serial.print("Z="); Serial.print(accelZ, 2); Serial.print(" m/s²  ");
  }
  
  if (sensor.isMagDataReady()) {
    sensor.readMag(magX, magY, magZ);
    Serial.print("Mag: ");
    Serial.print("X="); Serial.print(magX, 1); Serial.print(" ");
    Serial.print("Y="); Serial.print(magY, 1); Serial.print(" ");
    Serial.print("Z="); Serial.print(magZ, 1); Serial.print(" µT  ");
  }
  
  temperature = sensor.readTemperature();
  Serial.print("Temp: "); Serial.print(temperature, 1); Serial.println(" °C");
  
  delay(500);
}
