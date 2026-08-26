/**
 * Magnetometer_Calibration Example
 * 
 * Guides you through magnetometer calibration to compensate for hard-iron and soft-iron
 * distortions. These distortions are caused by nearby ferromagnetic materials (hard-iron)
 * and induced magnetic fields (soft-iron).
 * 
 * This example works with BOTH I2C and SPI modes - uncomment your preferred mode below.
 * 
 * Calibration Procedure:
 * 1. Upload this sketch
 * 2. Open Serial Monitor (115200 baud)
 * 3. When prompted, slowly rotate the sensor in all directions (figure-8 pattern)
 * 4. Continue for 30 seconds until data collection completes
 * 5. Calibration values will be displayed - save these for your application
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

// Calibration data
float magXMin = 9999, magXMax = -9999;
float magYMin = 9999, magYMax = -9999;
float magZMin = 9999, magZMax = -9999;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("Boardoza LSM303AGR - Magnetometer Calibration");
  Serial.println("===============================================");
  
#ifdef USE_SPI
  SPI.begin();
  if (!sensor.beginSPI(CS_ACCEL, CS_MAG)) {
#else
  Wire.begin();
  if (!sensor.begin()) {
#endif
    Serial.println("ERROR: Sensor not detected!");
    while (1) delay(10);
  }
  
#ifdef USE_SPI
  Serial.println("Mode: SPI");
#else
  Serial.println("Mode: I2C");
#endif
  
  // Configure magnetometer
  sensor.setMagMode(MAG_MODE_CONTINUOUS);
  sensor.setMagDataRate(50); // 50Hz for calibration
  sensor.setMagOffsetCancellation(false); // Disable internal compensation during calibration
  
  Serial.println();
  Serial.println("===========================================");
  Serial.println(" CALIBRATION PROCEDURE");
  Serial.println("===========================================");
  Serial.println("1. Slowly rotate the sensor in ALL directions");
  Serial.println("2. Make figure-8 patterns in the air");
  Serial.println("3. Ensure full 3D rotation coverage");
  Serial.println("4. Continue for 30 seconds");
  Serial.println();
  Serial.println("Starting in 3 seconds...");
  delay(3000);
  
  Serial.println("START ROTATING NOW!");
  Serial.println();
}

void loop() {
  static unsigned long startTime = millis();
  static unsigned long lastPrint = 0;
  static int sampleCount = 0;
  
  unsigned long elapsed = millis() - startTime;
  
  // Collect data for 30 seconds
  if (elapsed < 30000) {
    float magX, magY, magZ;
    
    if (sensor.isMagDataReady()) {
      sensor.readMag(magX, magY, magZ);
      
      // Update min/max values
      if (magX < magXMin) magXMin = magX;
      if (magX > magXMax) magXMax = magX;
      if (magY < magYMin) magYMin = magY;
      if (magY > magYMax) magYMax = magY;
      if (magZ < magZMin) magZMin = magZ;
      if (magZ > magZMax) magZMax = magZ;
      
      sampleCount++;
      
      // Print progress every second
      if (millis() - lastPrint >= 1000) {
        lastPrint = millis();
        int secondsRemaining = (30000 - elapsed) / 1000;
        Serial.print("Collecting... ");
        Serial.print(sampleCount);
        Serial.print(" samples  |  ");
        Serial.print(secondsRemaining);
        Serial.println(" seconds remaining");
      }
    }
  } else {
    // Calibration complete - calculate and display results
    Serial.println();
    Serial.println("===========================================");
    Serial.println(" CALIBRATION COMPLETE!");
    Serial.println("===========================================");
    Serial.print("Total samples collected: ");
    Serial.println(sampleCount);
    Serial.println();
    
    // Calculate hard-iron offsets (mid-point between min and max)
    float offsetX = (magXMax + magXMin) / 2.0f;
    float offsetY = (magYMax + magYMin) / 2.0f;
    float offsetZ = (magZMax + magZMin) / 2.0f;
    
    // Calculate soft-iron scale factors (range for each axis)
    float rangeX = magXMax - magXMin;
    float rangeY = magYMax - magYMin;
    float rangeZ = magZMax - magZMin;
    float avgRange = (rangeX + rangeY + rangeZ) / 3.0f;
    
    float scaleX = avgRange / rangeX;
    float scaleY = avgRange / rangeY;
    float scaleZ = avgRange / rangeZ;
    
    // Display raw min/max
    Serial.println("Raw Magnetometer Ranges:");
    Serial.print("  X: "); Serial.print(magXMin, 1); Serial.print(" to "); Serial.print(magXMax, 1); Serial.println(" µT");
    Serial.print("  Y: "); Serial.print(magYMin, 1); Serial.print(" to "); Serial.print(magYMax, 1); Serial.println(" µT");
    Serial.print("  Z: "); Serial.print(magZMin, 1); Serial.print(" to "); Serial.print(magZMax, 1); Serial.println(" µT");
    Serial.println();
    
    // Display calibration values
    Serial.println("🎯 CALIBRATION VALUES (copy these to your code):");
    Serial.println("------------------------------------------------");
    Serial.print("const float MAG_OFFSET_X = "); Serial.print(offsetX, 2); Serial.println(";");
    Serial.print("const float MAG_OFFSET_Y = "); Serial.print(offsetY, 2); Serial.println(";");
    Serial.print("const float MAG_OFFSET_Z = "); Serial.print(offsetZ, 2); Serial.println(";");
    Serial.println();
    Serial.print("const float MAG_SCALE_X = "); Serial.print(scaleX, 4); Serial.println(";");
    Serial.print("const float MAG_SCALE_Y = "); Serial.print(scaleY, 4); Serial.println(";");
    Serial.print("const float MAG_SCALE_Z = "); Serial.print(scaleZ, 4); Serial.println(";");
    Serial.println("------------------------------------------------");
    Serial.println();
    
    Serial.println("How to use in your code:");
    Serial.println("  float magX, magY, magZ;");
    Serial.println("  sensor.readMag(magX, magY, magZ);");
    Serial.println("  ");
    Serial.println("  // Apply calibration");
    Serial.println("  magX = (magX - MAG_OFFSET_X) * MAG_SCALE_X;");
    Serial.println("  magY = (magY - MAG_OFFSET_Y) * MAG_SCALE_Y;");
    Serial.println("  magZ = (magZ - MAG_OFFSET_Z) * MAG_SCALE_Z;");
    Serial.println();
    
    // Demonstrate calibrated readings
    Serial.println("Showing calibrated readings (press reset to recalibrate):");
    Serial.println();
    
    while (1) {
      float magX, magY, magZ;
      if (sensor.isMagDataReady()) {
        sensor.readMag(magX, magY, magZ);
        
        // Apply calibration
        magX = (magX - offsetX) * scaleX;
        magY = (magY - offsetY) * scaleY;
        magZ = (magZ - offsetZ) * scaleZ;
        
        // Calculate heading (0-360 degrees)
        float heading = atan2(magY, magX) * 180.0f / PI;
        if (heading < 0) heading += 360;
        
        Serial.print("Calibrated: X=");
        Serial.print(magX, 1);
        Serial.print(" Y=");
        Serial.print(magY, 1);
        Serial.print(" Z=");
        Serial.print(magZ, 1);
        Serial.print(" µT  |  Heading: ");
        Serial.print(heading, 1);
        Serial.println("°");
        
        delay(200);
      }
    }
  }
}
