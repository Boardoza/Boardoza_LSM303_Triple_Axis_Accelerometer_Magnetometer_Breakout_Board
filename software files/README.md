# Boardoza LSM303AGR Library

[![Arduino](https://img.shields.io/badge/Arduino-Compatible-brightgreen)](https://www.arduino.cc/)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](https://opensource.org/licenses/MIT)
[![Version](https://img.shields.io/badge/version-1.0.0-orange)](https://github.com/Boardoza/Boardoza_LSM303AGR_Library)

Ultra-compact 3D accelerometer and 3D magnetometer (eCompass) library for the STMicroelectronics **LSM303AGR** sensor.

## 🚀 Competitive Advantage

**First Arduino library with FULL I2C + SPI support for LSM303AGR!**

Unlike other LSM303AGR libraries that only support I2C, Boardoza provides:
- ✅ **Dual Protocol Support**: Choose I2C or SPI based on your application needs
- ✅ **25x Faster Data Transfer**: SPI operates at up to 10 MHz vs 400 kHz I2C Fast Mode
- ✅ **Unified API**: Same functions work regardless of I2C or SPI initialization
- ✅ **Flexible Integration**: Use I2C when pin count is limited, SPI when speed matters

## Features

- 🔄 **Dual Communication Protocol**: I2C and SPI (unique in Arduino ecosystem)
- 📊 **3-Axis Accelerometer**: ±2g to ±16g selectable range, 1-400Hz output data rate
- 🧭 **3-Axis Magnetometer**: ±50 gauss range, 10-100Hz output data rate
- 🌡️ **Temperature Sensor**: Integrated temperature measurement
- ⚡ **Power Management**: Low-power and high-resolution modes
- 🔍 **Built-in Self-Test**: Hardware verification for both accelerometer and magnetometer
- 🎯 **Calibration Support**: Example code for hard-iron and soft-iron compensation
- 📚 **4 Complete Examples**: I2C, SPI, calibration, and self-test

## Hardware Specifications

| Parameter | Value |
|-----------|-------|
| Operating Voltage | 1.71V to 3.6V |
| Operating Temperature | -40°C to +85°C |
| Accelerometer Range | ±2g, ±4g, ±8g, ±16g (user-selectable) |
| Magnetometer Range | ±50 gauss |
| Accelerometer ODR | 1Hz to 400Hz |
| Magnetometer ODR | 10Hz to 100Hz |
| I2C Addresses | 0x19 (Accel), 0x1E (Mag) |
| SPI Mode | Mode 3 (CPOL=1, CPHA=1) |
| SPI Clock Speed | 1 MHz to 10 MHz |

## Installation

### Arduino IDE
1. Download this repository as ZIP
2. In Arduino IDE: **Sketch** → **Include Library** → **Add .ZIP Library**
3. Select the downloaded ZIP file
4. Restart Arduino IDE

### PlatformIO
```ini
lib_deps = 
    boardoza/Boardoza LSM303AGR@^1.0.0
```

## Quick Start

### I2C Mode (Simple)
```cpp
#include <Wire.h>
#include <LSM303AGR.h>

LSM303AGR sensor;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  if (!sensor.begin()) {
    Serial.println("Sensor not found!");
    while (1);
  }
  
  sensor.setAccelRange(2);   // ±2g
  sensor.setMagMode(MAG_MODE_CONTINUOUS);
}

void loop() {
  float ax, ay, az, mx, my, mz;
  
  sensor.readAccel(ax, ay, az);  // m/s²
  sensor.readMag(mx, my, mz);    // µT
  
  Serial.print("Accel: "); 
  Serial.print(ax); Serial.print(", ");
  Serial.print(ay); Serial.print(", ");
  Serial.println(az);
  
  delay(100);
}
```

### SPI Mode (High-Speed)
```cpp
#include <SPI.h>
#include <LSM303AGR.h>

#define CS_ACCEL 10
#define CS_MAG   9

LSM303AGR sensor;

void setup() {
  Serial.begin(115200);
  SPI.begin();
  
  if (!sensor.beginSPI(CS_ACCEL, CS_MAG)) {
    Serial.println("Sensor not found!");
    while (1);
  }
  
  sensor.setSPIClockSpeed(10000000);  // 10 MHz for max speed
  sensor.setAccelRange(2);
  sensor.setAccelDataRate(400);       // 400Hz for high-speed logging
  sensor.setMagMode(MAG_MODE_CONTINUOUS);
}

void loop() {
  float ax, ay, az, mx, my, mz;
  
  if (sensor.isAccelDataReady()) {
    sensor.readAccel(ax, ay, az);
    // Process at 400Hz!
  }
  
  delay(10);
}
```

## Wiring

### I2C Wiring
```
LSM303AGR    Arduino Uno/Nano    Arduino Mega    ESP32
──────────────────────────────────────────────────────────
VDD          3.3V                3.3V            3.3V
GND          GND                 GND             GND
SDA          A4                  20              21 (GPIO21)
SCL          A5                  21              22 (GPIO22)
CS_ACCEL     3.3V (tied high)    3.3V            3.3V
CS_MAG       3.3V (tied high)    3.3V            3.3V
```

**Note**: I2C requires pull-up resistors (4.7kΩ recommended) on SDA and SCL lines.

### SPI Wiring
```
LSM303AGR    Arduino Uno/Nano    Arduino Mega    ESP32
──────────────────────────────────────────────────────────
VDD          3.3V                3.3V            3.3V
GND          GND                 GND             GND
MOSI         11                  51              23 (GPIO23)
MISO         12                  50              19 (GPIO19)
SCK          13                  52              18 (GPIO18)
CS_ACCEL     10 (user-defined)   10              5  (GPIO5)
CS_MAG       9  (user-defined)   9               4  (GPIO4)
SDA          N/C                 N/C             N/C
SCL          N/C                 N/C             N/C
```

**Note**: SPI requires separate chip select (CS) pins for accelerometer and magnetometer.

## I2C vs SPI Performance Comparison

| Feature | I2C | SPI |
|---------|-----|-----|
| Max Clock Speed | 400 kHz (Fast Mode) | 10 MHz |
| Data Transfer Rate | ~50 KB/s | ~1.25 MB/s |
| Pin Count | 2 (SDA, SCL) | 5 (MOSI, MISO, SCK, CS_ACCEL, CS_MAG) |
| Multi-Device Bus | Yes (addressable) | No (requires separate CS pins) |
| Speed Advantage | Baseline (1x) | **25x faster** |
| Recommended Use | Low pin count, multiple sensors | High-speed logging, real-time control |

**Use I2C when:**
- Pin count is limited
- Sharing bus with other I2C devices
- Speed is not critical (<100Hz sampling)

**Use SPI when:**
- Maximum throughput is required (>100Hz)
- Real-time data logging applications
- Minimizing latency is critical
- Dedicated communication bus available

## API Reference

### Initialization
- `bool begin(TwoWire *wire = &Wire)` - Initialize in I2C mode
- `bool beginSPI(uint8_t csAccel, uint8_t csMag, SPIClass *spi = &SPI)` - Initialize in SPI mode
- `void setSPIClockSpeed(uint32_t speed)` - Set SPI clock (1-10 MHz)

### Accelerometer
- `bool setAccelRange(uint8_t range)` - Set range: 2, 4, 8, or 16 (±g)
- `bool setAccelDataRate(uint16_t rate)` - Set ODR: 1, 10, 25, 50, 100, 200, 400 (Hz)
- `bool enableAccelAxes(bool x, bool y, bool z)` - Enable/disable axes
- `bool setAccelPowerMode(LSM303AGR_PowerMode mode)` - Low-power or high-resolution
- `bool setAccelEnabled(bool enabled)` - Power up/down accelerometer
- `bool readAccel(float &x, float &y, float &z)` - Read acceleration (m/s²)
- `bool isAccelDataReady()` - Check if new data available

### Magnetometer
- `bool setMagMode(LSM303AGR_MagMode mode)` - Continuous, single-shot, or idle
- `bool setMagDataRate(uint16_t rate)` - Set ODR: 10, 20, 50, 100 (Hz)
- `bool setMagOffsetCancellation(bool enabled)` - Hard-iron compensation
- `bool readMag(float &x, float &y, float &z)` - Read magnetic field (µT)
- `bool isMagDataReady()` - Check if new data available

### Temperature
- `bool enableTemperature(bool enabled)` - Enable/disable temperature sensor
- `float readTemperature()` - Read temperature (°C, ±2°C accuracy)

### Self-Test
- `bool runAccelSelfTest()` - Verify accelerometer operation
- `bool runMagSelfTest()` - Verify magnetometer operation

### Protocol Query
- `bool isI2C()` - Check if initialized in I2C mode
- `bool isSPI()` - Check if initialized in SPI mode

## Examples

### 1. Basic_Reading_I2C
Simple I2C example reading accelerometer, magnetometer, and temperature.

### 2. Basic_Reading_SPI ⚡
High-speed SPI example demonstrating competitive advantage with 400Hz sampling.

### 3. Magnetometer_Calibration
Interactive calibration procedure for hard-iron and soft-iron compensation. Works with both I2C and SPI.

### 4. Self_Test
Hardware verification using built-in self-test features. Works with both I2C and SPI.

## Calibration

Magnetometer readings benefit from calibration to compensate for:
- **Hard-iron distortion**: Nearby ferromagnetic materials (e.g., screws, batteries)
- **Soft-iron distortion**: Induced magnetic fields from PCB traces, power circuits

Run the `Magnetometer_Calibration` example to obtain calibration coefficients. Apply in your code:

```cpp
// From calibration example
const float MAG_OFFSET_X = -12.34;
const float MAG_OFFSET_Y = 5.67;
const float MAG_OFFSET_Z = -8.90;
const float MAG_SCALE_X = 1.05;
const float MAG_SCALE_Y = 0.98;
const float MAG_SCALE_Z = 1.02;

float mx, my, mz;
sensor.readMag(mx, my, mz);

// Apply calibration
mx = (mx - MAG_OFFSET_X) * MAG_SCALE_X;
my = (my - MAG_OFFSET_Y) * MAG_SCALE_Y;
mz = (mz - MAG_OFFSET_Z) * MAG_SCALE_Z;
```

## Troubleshooting

**Sensor not detected (begin() returns false)**
- Check wiring connections
- Verify I2C addresses (0x19 for accelerometer, 0x1E for magnetometer)
- Ensure pull-up resistors on SDA/SCL (I2C mode)
- Check power supply (1.71V-3.6V)

**Erratic magnetometer readings**
- Move away from magnetic interference sources
- Run magnetometer calibration
- Enable offset cancellation: `sensor.setMagOffsetCancellation(true)`

**SPI communication failure**
- Verify CS pins are correctly connected and specified
- Check SPI mode (should be Mode 3)
- Ensure SPI clock speed is within 1-10 MHz range

**Self-test fails**
- Check for mechanical damage to sensor
- Verify electrical connections
- Try different sensor module
- Review self-test threshold criteria in example code

## License

This library is released under the MIT License. See LICENSE file for details.

## About Boardoza

Boardoza creates professional-grade Arduino libraries with a focus on:
- 🎯 **Complete Feature Coverage**: Full API implementation, not just basics
- 🚀 **Performance**: Advanced features like SPI support for maximum throughput
- 📖 **Documentation**: Comprehensive examples and clear API reference
- 🔧 **Professional Standards**: Follows industry best practices

Visit [Boardoza.com](https://boardoza.com) for more libraries and embedded solutions.

## Contributing

Contributions are welcome! Please:
1. Fork the repository
2. Create a feature branch
3. Follow the existing code style (Doxygen comments in .cpp only)
4. Test on hardware
5. Submit a pull request

## Support

- **Issues**: [GitHub Issues](https://github.com/Boardoza/Boardoza_LSM303AGR_Library/issues)
- **Email**: eray.eroglu@boardoza.com
- **Website**: [Boardoza.com](https://boardoza.com)

## Credits

Developed by Boardoza with reference to:
- STMicroelectronics LSM303AGR datasheet (DocID027765 Rev 3)
- STMicroelectronics application notes AN4602, AN4508

## Changelog

### Version 1.0.0 (Initial Release)
- ✅ Complete I2C implementation
- ✅ Complete SPI implementation (industry-first for LSM303AGR)
- ✅ Unified protocol-agnostic API
- ✅ Accelerometer with 4 ranges, 7 data rates
- ✅ Magnetometer with continuous/single-shot modes
- ✅ Temperature sensor support
- ✅ Built-in self-test routines
- ✅ 4 comprehensive examples
- ✅ Full Doxygen documentation
