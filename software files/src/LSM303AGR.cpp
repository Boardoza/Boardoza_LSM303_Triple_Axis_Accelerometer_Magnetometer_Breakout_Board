#include "LSM303AGR.h"

/**
 * @brief Constructor for LSM303AGR class
 * 
 * Initializes member variables to default values. The sensor is not configured
 * until begin() or beginSPI() is called.
 */
LSM303AGR::LSM303AGR() {
  _wire = nullptr;
  _spi = nullptr;
  _csAccel = 0;
  _csMag = 0;
  _spiSpeed = 1000000; // 1 MHz default
  _protocol = PROTOCOL_I2C;
  _accelRange = 2;
  _accelScale = 1.0f;
}

/**
 * @brief Initialize the LSM303AGR sensor in I2C mode
 * 
 * This method configures the sensor for I2C communication, verifies both
 * accelerometer and magnetometer are present via WHO_AM_I register checks,
 * and applies default configuration settings.
 * 
 * Default configuration:
 * - Accelerometer: ±2g range, 10Hz ODR, high-resolution mode, all axes enabled
 * - Magnetometer: Continuous mode, 10Hz ODR, offset cancellation off
 * 
 * @param wire Pointer to TwoWire object for I2C communication (default: &Wire)
 * @return true if both sensors detected and configured successfully, false otherwise
 */
bool LSM303AGR::begin(TwoWire* wire) {
  _wire = wire;
  _protocol = PROTOCOL_I2C;
  
  _wire->begin();
  
  // Verify accelerometer WHO_AM_I
  uint8_t whoAmIA = readAccelRegI2C(LSM303AGR_WHO_AM_I_A);
  if (whoAmIA != LSM303AGR_WHO_AM_I_A_VALUE) {
    return false;
  }
  
  // Verify magnetometer WHO_AM_I
  uint8_t whoAmIM = readMagRegI2C(LSM303AGR_WHO_AM_I_M);
  if (whoAmIM != LSM303AGR_WHO_AM_I_M_VALUE) {
    return false;
  }
  
  // Configure accelerometer: 10Hz ODR, high-resolution mode, all axes enabled
  writeAccelRegI2C(LSM303AGR_CTRL_REG1_A, 0x27); // ODR=10Hz (0010), all axes enabled (0111)
  writeAccelRegI2C(LSM303AGR_CTRL_REG4_A, 0x08); // High-resolution mode, ±2g range
  
  _accelRange = 2;
  updateAccelScale();
  
  // Configure magnetometer: Continuous mode, 10Hz ODR
  writeMagRegI2C(LSM303AGR_CFG_REG_A_M, 0x00); // Continuous mode, 10Hz
  writeMagRegI2C(LSM303AGR_CFG_REG_B_M, 0x00); // Offset cancellation off
  writeMagRegI2C(LSM303AGR_CFG_REG_C_M, 0x00); // Continuous mode enable
  
  return true;
}

/**
 * @brief Update accelerometer scale factor based on current range setting
 * 
 * This internal method calculates the conversion factor from LSB to m/s²
 * based on the configured full-scale range.
 */
void LSM303AGR::updateAccelScale() {
  // LSM303AGR sensitivity in mg/LSB for different ranges in high-resolution mode
  // ±2g: ~1 mg/LSB, ±4g: ~2 mg/LSB, ±8g: ~4 mg/LSB, ±16g: ~12 mg/LSB
  float mgPerLSB;
  switch (_accelRange) {
    case 2:  mgPerLSB = 1.0f; break;
    case 4:  mgPerLSB = 2.0f; break;
    case 8:  mgPerLSB = 4.0f; break;
    case 16: mgPerLSB = 12.0f; break;
    default: mgPerLSB = 1.0f;
  }
  // Convert mg/LSB to m/s² per LSB: (mg/LSB) * (g/1000mg) * (9.80665 m/s² per g)
  _accelScale = mgPerLSB * 0.00980665f;
}

// ========== I2C Register Access Helpers ==========

/**
 * @brief Write a byte to an accelerometer register via I2C
 * 
 * @param reg Register address
 * @param value Value to write
 */
void LSM303AGR::writeAccelRegI2C(uint8_t reg, uint8_t value) {
  _wire->beginTransmission(LSM303AGR_ACCEL_ADDRESS);
  _wire->write(reg);
  _wire->write(value);
  _wire->endTransmission();
}

/**
 * @brief Read a byte from an accelerometer register via I2C
 * 
 * @param reg Register address
 * @return Value read from register
 */
uint8_t LSM303AGR::readAccelRegI2C(uint8_t reg) {
  _wire->beginTransmission(LSM303AGR_ACCEL_ADDRESS);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom(LSM303AGR_ACCEL_ADDRESS, (uint8_t)1);
  return _wire->read();
}

/**
 * @brief Write a byte to a magnetometer register via I2C
 * 
 * @param reg Register address
 * @param value Value to write
 */
void LSM303AGR::writeMagRegI2C(uint8_t reg, uint8_t value) {
  _wire->beginTransmission(LSM303AGR_MAG_ADDRESS);
  _wire->write(reg);
  _wire->write(value);
  _wire->endTransmission();
}

/**
 * @brief Read a byte from a magnetometer register via I2C
 * 
 * @param reg Register address
 * @return Value read from register
 */
uint8_t LSM303AGR::readMagRegI2C(uint8_t reg) {
  _wire->beginTransmission(LSM303AGR_MAG_ADDRESS);
  _wire->write(reg);
  _wire->endTransmission(false);
  _wire->requestFrom(LSM303AGR_MAG_ADDRESS, (uint8_t)1);
  return _wire->read();
}

/**
 * @brief Read multiple bytes from consecutive registers via I2C
 * 
 * This method uses I2C auto-increment to efficiently read multiple consecutive
 * registers in a single transaction.
 * 
 * @param address I2C device address
 * @param reg Starting register address
 * @param buffer Buffer to store read data
 * @param length Number of bytes to read
 */
void LSM303AGR::readMultiI2C(uint8_t address, uint8_t reg, uint8_t* buffer, uint8_t length) {
  _wire->beginTransmission(address);
  _wire->write(reg | 0x80); // Set auto-increment bit
  _wire->endTransmission(false);
  _wire->requestFrom(address, length);
  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = _wire->read();
  }
}

/**
 * @brief Check if communication protocol is I2C
 * 
 * @return true if using I2C, false if using SPI
 */
bool LSM303AGR::isI2C() {
  return _protocol == PROTOCOL_I2C;
}

/**
 * @brief Check if communication protocol is SPI
 * 
 * @return true if using SPI, false if using I2C
 */
bool LSM303AGR::isSPI() {
  return _protocol == PROTOCOL_SPI;
}

// ========== SPI Initialization ==========

/**
 * @brief Initialize the LSM303AGR sensor in SPI mode
 * 
 * This method configures the sensor for SPI communication using separate chip select
 * pins for the accelerometer and magnetometer. It verifies both sensors are present
 * via WHO_AM_I register checks and applies default configuration settings.
 * 
 * SPI Configuration: Mode 3 (CPOL=1, CPHA=1), MSB first, 1 MHz default clock speed
 * 
 * Default configuration:
 * - Accelerometer: ±2g range, 10Hz ODR, high-resolution mode, all axes enabled
 * - Magnetometer: Continuous mode, 10Hz ODR, offset cancellation off
 * 
 * @param csAccel Chip select pin for accelerometer
 * @param csMag Chip select pin for magnetometer
 * @param spi Pointer to SPIClass object for SPI communication (default: &SPI)
 * @return true if both sensors detected and configured successfully, false otherwise
 */
bool LSM303AGR::beginSPI(uint8_t csAccel, uint8_t csMag, SPIClass* spi) {
  _spi = spi;
  _csAccel = csAccel;
  _csMag = csMag;
  _protocol = PROTOCOL_SPI;
  
  // Configure chip select pins
  pinMode(_csAccel, OUTPUT);
  pinMode(_csMag, OUTPUT);
  digitalWrite(_csAccel, HIGH); // Inactive
  digitalWrite(_csMag, HIGH);   // Inactive
  
  // Initialize SPI with Mode 3, MSB first, 1 MHz default
  _spi->begin();
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  _spi->endTransaction();
  
  // Verify accelerometer WHO_AM_I
  uint8_t whoAmIA = readAccelRegSPI(LSM303AGR_WHO_AM_I_A);
  if (whoAmIA != LSM303AGR_WHO_AM_I_A_VALUE) {
    return false;
  }
  
  // Verify magnetometer WHO_AM_I
  uint8_t whoAmIM = readMagRegSPI(LSM303AGR_WHO_AM_I_M);
  if (whoAmIM != LSM303AGR_WHO_AM_I_M_VALUE) {
    return false;
  }
  
  // Configure accelerometer: 10Hz ODR, high-resolution mode, all axes enabled
  writeAccelRegSPI(LSM303AGR_CTRL_REG1_A, 0x27); // ODR=10Hz (0010), all axes enabled (0111)
  writeAccelRegSPI(LSM303AGR_CTRL_REG4_A, 0x08); // High-resolution mode, ±2g range
  
  _accelRange = 2;
  updateAccelScale();
  
  // Configure magnetometer: Continuous mode, 10Hz ODR
  writeMagRegSPI(LSM303AGR_CFG_REG_A_M, 0x00); // Continuous mode, 10Hz
  writeMagRegSPI(LSM303AGR_CFG_REG_B_M, 0x00); // Offset cancellation off
  writeMagRegSPI(LSM303AGR_CFG_REG_C_M, 0x00); // Continuous mode enable
  
  return true;
}

/**
 * @brief Set custom SPI clock speed
 * 
 * Allows setting SPI clock speed between 1 MHz and 10 MHz (LSM303AGR maximum).
 * Higher speeds enable faster data acquisition at the cost of potential signal
 * integrity issues on longer wires or noisy environments.
 * 
 * @param speed SPI clock speed in Hz (1000000 to 10000000)
 */
void LSM303AGR::setSPIClockSpeed(uint32_t speed) {
  if (speed < 1000000) speed = 1000000;   // Minimum 1 MHz
  if (speed > 10000000) speed = 10000000; // Maximum 10 MHz
  _spiSpeed = speed;
}

// ========== SPI Register Access Helpers ==========

/**
 * @brief Write a byte to an accelerometer register via SPI
 * 
 * @param reg Register address
 * @param value Value to write
 */
void LSM303AGR::writeAccelRegSPI(uint8_t reg, uint8_t value) {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csAccel, LOW);
  _spi->transfer(reg & 0x7F); // Clear read bit (write mode)
  _spi->transfer(value);
  digitalWrite(_csAccel, HIGH);
  _spi->endTransaction();
}

/**
 * @brief Read a byte from an accelerometer register via SPI
 * 
 * @param reg Register address
 * @return Value read from register
 */
uint8_t LSM303AGR::readAccelRegSPI(uint8_t reg) {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csAccel, LOW);
  _spi->transfer(reg | LSM303AGR_SPI_READ); // Set read bit
  uint8_t value = _spi->transfer(0x00);
  digitalWrite(_csAccel, HIGH);
  _spi->endTransaction();
  return value;
}

/**
 * @brief Write a byte to a magnetometer register via SPI
 * 
 * @param reg Register address
 * @param value Value to write
 */
void LSM303AGR::writeMagRegSPI(uint8_t reg, uint8_t value) {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csMag, LOW);
  _spi->transfer(reg & 0x7F); // Clear read bit (write mode)
  _spi->transfer(value);
  digitalWrite(_csMag, HIGH);
  _spi->endTransaction();
}

/**
 * @brief Read a byte from a magnetometer register via SPI
 * 
 * @param reg Register address
 * @return Value read from register
 */
uint8_t LSM303AGR::readMagRegSPI(uint8_t reg) {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(_csMag, LOW);
  _spi->transfer(reg | LSM303AGR_SPI_READ); // Set read bit
  uint8_t value = _spi->transfer(0x00);
  digitalWrite(_csMag, HIGH);
  _spi->endTransaction();
  return value;
}

/**
 * @brief Read multiple bytes from consecutive registers via SPI
 * 
 * This method uses SPI auto-increment to efficiently read multiple consecutive
 * registers in a single transaction.
 * 
 * @param cs Chip select pin to use
 * @param reg Starting register address
 * @param buffer Buffer to store read data
 * @param length Number of bytes to read
 */
void LSM303AGR::readMultiSPI(uint8_t cs, uint8_t reg, uint8_t* buffer, uint8_t length) {
  _spi->beginTransaction(SPISettings(_spiSpeed, MSBFIRST, SPI_MODE3));
  digitalWrite(cs, LOW);
  _spi->transfer(reg | LSM303AGR_SPI_READ | LSM303AGR_SPI_AUTO_INC); // Read + auto-increment
  for (uint8_t i = 0; i < length; i++) {
    buffer[i] = _spi->transfer(0x00);
  }
  digitalWrite(cs, HIGH);
  _spi->endTransaction();
}

// ========== Protocol-Agnostic Register Access ==========

/**
 * @brief Write to accelerometer register (protocol-agnostic)
 * 
 * Automatically routes to I2C or SPI based on initialization.
 * 
 * @param reg Register address
 * @param value Value to write
 */
void LSM303AGR::writeAccelReg(uint8_t reg, uint8_t value) {
  if (_protocol == PROTOCOL_I2C) {
    writeAccelRegI2C(reg, value);
  } else {
    writeAccelRegSPI(reg, value);
  }
}

/**
 * @brief Read from accelerometer register (protocol-agnostic)
 * 
 * Automatically routes to I2C or SPI based on initialization.
 * 
 * @param reg Register address
 * @return Value read from register
 */
uint8_t LSM303AGR::readAccelReg(uint8_t reg) {
  if (_protocol == PROTOCOL_I2C) {
    return readAccelRegI2C(reg);
  } else {
    return readAccelRegSPI(reg);
  }
}

/**
 * @brief Write to magnetometer register (protocol-agnostic)
 * 
 * Automatically routes to I2C or SPI based on initialization.
 * 
 * @param reg Register address
 * @param value Value to write
 */
void LSM303AGR::writeMagReg(uint8_t reg, uint8_t value) {
  if (_protocol == PROTOCOL_I2C) {
    writeMagRegI2C(reg, value);
  } else {
    writeMagRegSPI(reg, value);
  }
}

/**
 * @brief Read from magnetometer register (protocol-agnostic)
 * 
 * Automatically routes to I2C or SPI based on initialization.
 * 
 * @param reg Register address
 * @return Value read from register
 */
uint8_t LSM303AGR::readMagReg(uint8_t reg) {
  if (_protocol == PROTOCOL_I2C) {
    return readMagRegI2C(reg);
  } else {
    return readMagRegSPI(reg);
  }
}

// ========== Accelerometer Methods ==========

/**
 * @brief Set accelerometer full-scale range
 * 
 * Configures the measurement range of the accelerometer. Larger ranges reduce
 * sensitivity but allow measuring higher accelerations.
 * 
 * @param range Full-scale range in g (2, 4, 8, or 16)
 * @return true if successful, false if invalid range
 */
bool LSM303AGR::setAccelRange(uint8_t range) {
  uint8_t rangeValue;
  switch (range) {
    case 2:  rangeValue = ACCEL_RANGE_2G; break;
    case 4:  rangeValue = ACCEL_RANGE_4G; break;
    case 8:  rangeValue = ACCEL_RANGE_8G; break;
    case 16: rangeValue = ACCEL_RANGE_16G; break;
    default: return false;
  }
  
  uint8_t ctrl4 = readAccelReg(LSM303AGR_CTRL_REG4_A);
  ctrl4 = (ctrl4 & 0xCF) | rangeValue;
  writeAccelReg(LSM303AGR_CTRL_REG4_A, ctrl4);
  
  _accelRange = range;
  updateAccelScale();
  return true;
}

/**
 * @brief Set accelerometer output data rate
 * 
 * Configures how frequently the accelerometer updates its output registers.
 * Higher rates enable faster sampling but increase power consumption.
 * 
 * @param rate Output data rate in Hz (1, 10, 25, 50, 100, 200, or 400)
 * @return true if successful, false if invalid rate
 */
bool LSM303AGR::setAccelDataRate(uint16_t rate) {
  uint8_t odrValue;
  switch (rate) {
    case 1:   odrValue = ACCEL_ODR_1HZ; break;
    case 10:  odrValue = ACCEL_ODR_10HZ; break;
    case 25:  odrValue = ACCEL_ODR_25HZ; break;
    case 50:  odrValue = ACCEL_ODR_50HZ; break;
    case 100: odrValue = ACCEL_ODR_100HZ; break;
    case 200: odrValue = ACCEL_ODR_200HZ; break;
    case 400: odrValue = ACCEL_ODR_400HZ; break;
    default: return false;
  }
  
  uint8_t ctrl1 = readAccelReg(LSM303AGR_CTRL_REG1_A);
  ctrl1 = (ctrl1 & 0x0F) | odrValue;
  writeAccelReg(LSM303AGR_CTRL_REG1_A, ctrl1);
  return true;
}

/**
 * @brief Enable or disable accelerometer axes
 * 
 * Allows selective enabling of X, Y, and Z axes to reduce power consumption
 * when only certain axes are needed.
 * 
 * @param x Enable X axis
 * @param y Enable Y axis
 * @param z Enable Z axis
 * @return true if successful
 */
bool LSM303AGR::enableAccelAxes(bool x, bool y, bool z) {
  uint8_t ctrl1 = readAccelReg(LSM303AGR_CTRL_REG1_A);
  ctrl1 &= 0xF8; // Clear axis enable bits
  if (x) ctrl1 |= 0x01;
  if (y) ctrl1 |= 0x02;
  if (z) ctrl1 |= 0x04;
  writeAccelReg(LSM303AGR_CTRL_REG1_A, ctrl1);
  return true;
}

/**
 * @brief Set accelerometer power mode
 * 
 * Configures power vs. performance trade-off. Low-power mode reduces current
 * consumption but provides 8-bit resolution. High-resolution mode provides
 * 12-bit resolution with higher current consumption.
 * 
 * @param mode Power mode (POWER_MODE_LOW_POWER or POWER_MODE_HIGH_RESOLUTION)
 * @return true if successful
 */
bool LSM303AGR::setAccelPowerMode(LSM303AGR_PowerMode mode) {
  uint8_t ctrl1 = readAccelReg(LSM303AGR_CTRL_REG1_A);
  uint8_t ctrl4 = readAccelReg(LSM303AGR_CTRL_REG4_A);
  
  if (mode == POWER_MODE_LOW_POWER) {
    ctrl1 |= 0x08;  // Set LP bit
    ctrl4 &= ~0x08; // Clear HR bit
  } else {
    ctrl1 &= ~0x08; // Clear LP bit
    ctrl4 |= 0x08;  // Set HR bit
  }
  
  writeAccelReg(LSM303AGR_CTRL_REG1_A, ctrl1);
  writeAccelReg(LSM303AGR_CTRL_REG4_A, ctrl4);
  return true;
}

/**
 * @brief Enable or disable accelerometer
 * 
 * Powers the accelerometer up or down. Power-down mode minimizes current
 * consumption when measurements are not needed.
 * 
 * @param enabled true to enable, false to power down
 * @return true if successful
 */
bool LSM303AGR::setAccelEnabled(bool enabled) {
  if (enabled) {
    // Set to 10Hz ODR (minimum for operation)
    uint8_t ctrl1 = readAccelReg(LSM303AGR_CTRL_REG1_A);
    ctrl1 = (ctrl1 & 0x0F) | ACCEL_ODR_10HZ;
    writeAccelReg(LSM303AGR_CTRL_REG1_A, ctrl1);
  } else {
    // Set to power-down mode
    uint8_t ctrl1 = readAccelReg(LSM303AGR_CTRL_REG1_A);
    ctrl1 = (ctrl1 & 0x0F) | ACCEL_ODR_POWER_DOWN;
    writeAccelReg(LSM303AGR_CTRL_REG1_A, ctrl1);
  }
  return true;
}

/**
 * @brief Check if new accelerometer data is available
 * 
 * Reads the status register to determine if the accelerometer has completed
 * a new measurement since the last read.
 * 
 * @return true if new data available, false otherwise
 */
bool LSM303AGR::isAccelDataReady() {
  uint8_t status = readAccelReg(LSM303AGR_STATUS_REG_A);
  return (status & 0x08) != 0; // Check ZYXDA bit
}

/**
 * @brief Read 3-axis accelerometer data
 * 
 * Reads raw accelerometer data and converts to SI units (m/s²). The conversion
 * factor is automatically adjusted based on the configured full-scale range.
 * 
 * @param x Output: X-axis acceleration in m/s²
 * @param y Output: Y-axis acceleration in m/s²
 * @param z Output: Z-axis acceleration in m/s²
 * @return true if successful
 */
bool LSM303AGR::readAccel(float &x, float &y, float &z) {
  uint8_t buffer[6];
  
  if (_protocol == PROTOCOL_I2C) {
    readMultiI2C(LSM303AGR_ACCEL_ADDRESS, LSM303AGR_OUT_X_L_A, buffer, 6);
  } else {
    readMultiSPI(_csAccel, LSM303AGR_OUT_X_L_A, buffer, 6);
  }
  
  int16_t rawX = (int16_t)(buffer[1] << 8 | buffer[0]) >> 4; // 12-bit right-aligned
  int16_t rawY = (int16_t)(buffer[3] << 8 | buffer[2]) >> 4;
  int16_t rawZ = (int16_t)(buffer[5] << 8 | buffer[4]) >> 4;
  
  x = rawX * _accelScale;
  y = rawY * _accelScale;
  z = rawZ * _accelScale;
  
  return true;
}

// ========== Magnetometer Methods ==========

/**
 * @brief Set magnetometer operating mode
 * 
 * Configures the magnetometer for continuous measurements, single-shot
 * measurements, or idle mode for power savings.
 * 
 * @param mode Operating mode (MAG_MODE_CONTINUOUS, MAG_MODE_SINGLE_SHOT, or MAG_MODE_IDLE)
 * @return true if successful
 */
bool LSM303AGR::setMagMode(LSM303AGR_MagMode mode) {
  uint8_t cfgA = readMagReg(LSM303AGR_CFG_REG_A_M);
  cfgA = (cfgA & 0xFC) | mode;
  writeMagReg(LSM303AGR_CFG_REG_A_M, cfgA);
  return true;
}

/**
 * @brief Set magnetometer output data rate
 * 
 * Configures how frequently the magnetometer updates its output registers
 * in continuous mode.
 * 
 * @param rate Output data rate in Hz (10, 20, 50, or 100)
 * @return true if successful, false if invalid rate
 */
bool LSM303AGR::setMagDataRate(uint16_t rate) {
  uint8_t odrValue;
  switch (rate) {
    case 10:  odrValue = MAG_ODR_10HZ; break;
    case 20:  odrValue = MAG_ODR_20HZ; break;
    case 50:  odrValue = MAG_ODR_50HZ; break;
    case 100: odrValue = MAG_ODR_100HZ; break;
    default: return false;
  }
  
  uint8_t cfgA = readMagReg(LSM303AGR_CFG_REG_A_M);
  cfgA = (cfgA & 0xF3) | odrValue;
  writeMagReg(LSM303AGR_CFG_REG_A_M, cfgA);
  return true;
}

/**
 * @brief Enable or disable magnetometer offset cancellation
 * 
 * When enabled, the magnetometer applies internal offset cancellation to
 * compensate for hard iron effects. This feature helps improve accuracy
 * in the presence of nearby ferromagnetic materials.
 * 
 * @param enabled true to enable offset cancellation, false to disable
 * @return true if successful
 */
bool LSM303AGR::setMagOffsetCancellation(bool enabled) {
  uint8_t cfgB = readMagReg(LSM303AGR_CFG_REG_B_M);
  if (enabled) {
    cfgB |= 0x02; // Set offset cancellation bit
  } else {
    cfgB &= ~0x02; // Clear offset cancellation bit
  }
  writeMagReg(LSM303AGR_CFG_REG_B_M, cfgB);
  return true;
}

/**
 * @brief Check if new magnetometer data is available
 * 
 * Reads the status register to determine if the magnetometer has completed
 * a new measurement since the last read.
 * 
 * @return true if new data available, false otherwise
 */
bool LSM303AGR::isMagDataReady() {
  uint8_t status = readMagReg(LSM303AGR_STATUS_REG_M);
  return (status & 0x08) != 0; // Check ZYXDA bit
}

/**
 * @brief Read 3-axis magnetometer data
 * 
 * Reads raw magnetometer data and converts to SI units (µT - microtesla).
 * The LSM303AGR magnetometer has a fixed scale factor of 1.5 mG/LSB = 0.15 µT/LSB.
 * 
 * @param x Output: X-axis magnetic field in µT
 * @param y Output: Y-axis magnetic field in µT
 * @param z Output: Z-axis magnetic field in µT
 * @return true if successful
 */
bool LSM303AGR::readMag(float &x, float &y, float &z) {
  uint8_t buffer[6];
  
  if (_protocol == PROTOCOL_I2C) {
    readMultiI2C(LSM303AGR_MAG_ADDRESS, LSM303AGR_OUTX_L_REG_M, buffer, 6);
  } else {
    readMultiSPI(_csMag, LSM303AGR_OUTX_L_REG_M, buffer, 6);
  }
  
  int16_t rawX = (int16_t)(buffer[1] << 8 | buffer[0]);
  int16_t rawY = (int16_t)(buffer[3] << 8 | buffer[2]);
  int16_t rawZ = (int16_t)(buffer[5] << 8 | buffer[4]);
  
  // Scale factor: 1.5 mG/LSB = 0.15 µT/LSB
  const float scale = 0.15f;
  x = rawX * scale;
  y = rawY * scale;
  z = rawZ * scale;
  
  // Check for overflow
  uint8_t status = readMagReg(LSM303AGR_STATUS_REG_M);
  if (status & 0x70) { // Check XYZ overflow bits
    // Data may be saturated
  }
  
  return true;
}

// ========== Temperature Methods ==========

/**
 * @brief Enable or disable temperature sensor
 * 
 * The temperature sensor is integrated with the accelerometer and can be
 * powered independently to minimize power consumption.
 * 
 * @param enabled true to enable, false to disable
 * @return true if successful
 */
bool LSM303AGR::enableTemperature(bool enabled) {
  uint8_t tempCfg = readAccelReg(LSM303AGR_TEMP_CFG_REG_A);
  if (enabled) {
    tempCfg |= 0xC0; // Enable temperature sensor (TEMP_EN bits)
    
    // Also enable BDU (Block Data Update) in CTRL_REG4_A for synchronized reading
    uint8_t ctrl4 = readAccelReg(LSM303AGR_CTRL_REG4_A);
    ctrl4 |= 0x80; // Set BDU bit
    writeAccelReg(LSM303AGR_CTRL_REG4_A, ctrl4);
  } else {
    tempCfg &= ~0xC0; // Disable temperature sensor
  }
  writeAccelReg(LSM303AGR_TEMP_CFG_REG_A, tempCfg);
  return true;
}

/**
 * @brief Read temperature sensor
 * 
 * Reads the integrated temperature sensor and converts to degrees Celsius.
 * The sensor has typical accuracy of ±2°C and is intended for relative
 * temperature measurements and thermal compensation, not precision thermometry.
 * 
 * Note: Temperature sensor must be enabled via enableTemperature() before reading.
 * Temperature is sampled at the same rate as the accelerometer ODR.
 * 
 * @return Temperature in degrees Celsius
 */
float LSM303AGR::readTemperature() {
  uint8_t buffer[2];
  
  if (_protocol == PROTOCOL_I2C) {
    readMultiI2C(LSM303AGR_ACCEL_ADDRESS, LSM303AGR_TEMP_OUT_L_A, buffer, 2);
  } else {
    readMultiSPI(_csAccel, LSM303AGR_TEMP_OUT_L_A, buffer, 2);
  }
  
  // LSM303AGR temperature: 12-bit signed in 16-bit register
  // Output is left-aligned (12-bit value in MSBs, 4 LSBs are zeros)
  // At 25°C, the output is approximately 0
  // Sensitivity: approximately 8 LSB/°C (typical)
  int16_t rawTemp = (int16_t)(buffer[1] << 8 | buffer[0]);
  
  // Right-shift by 4 to get the actual 12-bit value (since it's left-aligned)
  rawTemp = rawTemp >> 4;
  
  // Convert to temperature: T = 25°C + (rawTemp / 8)
  // where 8 LSB/°C is the typical sensitivity
  float temperature = 25.0f + (rawTemp / 8.0f);
  
  return temperature;
}

// ========== Self-Test Methods ==========

/**
 * @brief Run accelerometer self-test
 * 
 * Executes the built-in accelerometer self-test sequence by enabling the
 * self-test stimulus, capturing readings, and comparing the difference to
 * expected ranges. The previous configuration is restored after the test.
 * 
 * The self-test applies an electrostatic force to the sensor and verifies
 * that the output changes by an expected amount, indicating the sensor is
 * functioning correctly.
 * 
 * @return true if self-test passes, false if it fails or sensor malfunction detected
 */
bool LSM303AGR::runAccelSelfTest() {
  // Save current configuration
  uint8_t ctrl1_backup = readAccelReg(LSM303AGR_CTRL_REG1_A);
  uint8_t ctrl4_backup = readAccelReg(LSM303AGR_CTRL_REG4_A);
  
  // Configure for self-test: ±2g, normal mode
  writeAccelReg(LSM303AGR_CTRL_REG1_A, 0x47); // 50Hz, all axes
  writeAccelReg(LSM303AGR_CTRL_REG4_A, 0x00); // ±2g
  delay(100); // Stabilization time
  
  // Read baseline
  float x1, y1, z1;
  readAccel(x1, y1, z1);
  
  // Enable self-test
  uint8_t ctrl4 = readAccelReg(LSM303AGR_CTRL_REG4_A);
  writeAccelReg(LSM303AGR_CTRL_REG4_A, ctrl4 | 0x02); // Self-test 0
  delay(100);
  
  // Read with self-test active
  float x2, y2, z2;
  readAccel(x2, y2, z2);
  
  // Disable self-test
  writeAccelReg(LSM303AGR_CTRL_REG4_A, ctrl4);
  
  // Restore configuration
  writeAccelReg(LSM303AGR_CTRL_REG1_A, ctrl1_backup);
  writeAccelReg(LSM303AGR_CTRL_REG4_A, ctrl4_backup);
  
  // Check if difference is within expected range (typically 60-1700 mg for ±2g)
  float dx = abs(x2 - x1);
  float dy = abs(y2 - y1);
  float dz = abs(z2 - z1);
  
  // Convert to mg for comparison (m/s² to mg: divide by 9.80665 * 1000)
  float dx_mg = dx / 0.00980665f;
  float dy_mg = dy / 0.00980665f;
  float dz_mg = dz / 0.00980665f;
  
  bool pass = (dx_mg > 60 && dx_mg < 1700) &&
              (dy_mg > 60 && dy_mg < 1700) &&
              (dz_mg > 60 && dz_mg < 1700);
  
  return pass;
}

/**
 * @brief Run magnetometer self-test
 * 
 * Executes the built-in magnetometer self-test sequence by enabling the
 * self-test field, capturing readings, and comparing the difference to
 * expected ranges. The previous configuration is restored after the test.
 * 
 * The self-test applies an internal magnetic field and verifies that the
 * output changes by an expected amount, indicating the sensor is functioning
 * correctly.
 * 
 * @return true if self-test passes, false if it fails or sensor malfunction detected
 */
bool LSM303AGR::runMagSelfTest() {
  // Save current configuration
  uint8_t cfgA_backup = readMagReg(LSM303AGR_CFG_REG_A_M);
  uint8_t cfgC_backup = readMagReg(LSM303AGR_CFG_REG_C_M);
  
  // Configure for self-test
  writeMagReg(LSM303AGR_CFG_REG_A_M, 0x0C); // 50Hz, continuous mode
  writeMagReg(LSM303AGR_CFG_REG_C_M, 0x00);
  delay(100);
  
  // Read baseline
  float x1, y1, z1;
  readMag(x1, y1, z1);
  
  // Enable self-test
  uint8_t cfgC = readMagReg(LSM303AGR_CFG_REG_C_M);
  writeMagReg(LSM303AGR_CFG_REG_C_M, cfgC | 0x02); // Self-test enable
  delay(100);
  
  // Read with self-test active
  float x2, y2, z2;
  readMag(x2, y2, z2);
  
  // Disable self-test
  writeMagReg(LSM303AGR_CFG_REG_C_M, cfgC);
  
  // Restore configuration
  writeMagReg(LSM303AGR_CFG_REG_A_M, cfgA_backup);
  writeMagReg(LSM303AGR_CFG_REG_C_M, cfgC_backup);
  
  // Check if difference is significant (magnetometer self-test should produce measurable change)
  float dx = abs(x2 - x1);
  float dy = abs(y2 - y1);
  float dz = abs(z2 - z1);
  
  // Self-test should produce at least 1.5 mT (1500 µT) difference
  bool pass = (dx > 1500 || dy > 1500 || dz > 1500);
  
  return pass;
}
