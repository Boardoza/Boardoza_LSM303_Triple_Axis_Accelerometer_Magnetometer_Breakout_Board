#ifndef LSM303AGR_H
#define LSM303AGR_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

// I2C Addresses
#define LSM303AGR_ACCEL_ADDRESS 0x19
#define LSM303AGR_MAG_ADDRESS   0x1E

// SPI Bit Masks
#define LSM303AGR_SPI_READ      0x80
#define LSM303AGR_SPI_WRITE     0x00
#define LSM303AGR_SPI_AUTO_INC  0x40

// Accelerometer Register Addresses
#define LSM303AGR_WHO_AM_I_A      0x0F
#define LSM303AGR_CTRL_REG1_A     0x20
#define LSM303AGR_CTRL_REG2_A     0x21
#define LSM303AGR_CTRL_REG3_A     0x22
#define LSM303AGR_CTRL_REG4_A     0x23
#define LSM303AGR_CTRL_REG5_A     0x24
#define LSM303AGR_CTRL_REG6_A     0x25
#define LSM303AGR_STATUS_REG_A    0x27
#define LSM303AGR_OUT_X_L_A       0x28
#define LSM303AGR_OUT_X_H_A       0x29
#define LSM303AGR_OUT_Y_L_A       0x2A
#define LSM303AGR_OUT_Y_H_A       0x2B
#define LSM303AGR_OUT_Z_L_A       0x2C
#define LSM303AGR_OUT_Z_H_A       0x2D
#define LSM303AGR_TEMP_CFG_REG_A  0x1F
#define LSM303AGR_TEMP_OUT_L_A    0x0C
#define LSM303AGR_TEMP_OUT_H_A    0x0D

// Magnetometer Register Addresses
#define LSM303AGR_WHO_AM_I_M      0x4F
#define LSM303AGR_CFG_REG_A_M     0x60
#define LSM303AGR_CFG_REG_B_M     0x61
#define LSM303AGR_CFG_REG_C_M     0x62
#define LSM303AGR_STATUS_REG_M    0x67
#define LSM303AGR_OUTX_L_REG_M    0x68
#define LSM303AGR_OUTX_H_REG_M    0x69
#define LSM303AGR_OUTY_L_REG_M    0x6A
#define LSM303AGR_OUTY_H_REG_M    0x6B
#define LSM303AGR_OUTZ_L_REG_M    0x6C
#define LSM303AGR_OUTZ_H_REG_M    0x6D

// WHO_AM_I Expected Values
#define LSM303AGR_WHO_AM_I_A_VALUE  0x33
#define LSM303AGR_WHO_AM_I_M_VALUE  0x40

// Communication Protocol
enum LSM303AGR_Protocol {
  PROTOCOL_I2C,
  PROTOCOL_SPI
};

// Accelerometer Full-Scale Range
enum LSM303AGR_AccelRange {
  ACCEL_RANGE_2G  = 0x00,
  ACCEL_RANGE_4G  = 0x10,
  ACCEL_RANGE_8G  = 0x20,
  ACCEL_RANGE_16G = 0x30
};

// Accelerometer Output Data Rate
enum LSM303AGR_AccelDataRate {
  ACCEL_ODR_POWER_DOWN = 0x00,
  ACCEL_ODR_1HZ        = 0x10,
  ACCEL_ODR_10HZ       = 0x20,
  ACCEL_ODR_25HZ       = 0x30,
  ACCEL_ODR_50HZ       = 0x40,
  ACCEL_ODR_100HZ      = 0x50,
  ACCEL_ODR_200HZ      = 0x60,
  ACCEL_ODR_400HZ      = 0x70
};

// Accelerometer Power Mode
enum LSM303AGR_PowerMode {
  POWER_MODE_LOW_POWER      = 0x00,
  POWER_MODE_HIGH_RESOLUTION = 0x08
};

// Magnetometer Operating Mode
enum LSM303AGR_MagMode {
  MAG_MODE_CONTINUOUS  = 0x00,
  MAG_MODE_SINGLE_SHOT = 0x01,
  MAG_MODE_IDLE        = 0x03
};

// Magnetometer Output Data Rate
enum LSM303AGR_MagDataRate {
  MAG_ODR_10HZ  = 0x00,
  MAG_ODR_20HZ  = 0x04,
  MAG_ODR_50HZ  = 0x08,
  MAG_ODR_100HZ = 0x0C
};

class LSM303AGR {
public:
  LSM303AGR();
  
  bool begin(TwoWire* wire = &Wire);
  bool beginSPI(uint8_t csAccel, uint8_t csMag, SPIClass* spi = &SPI);
  
  void setSPIClockSpeed(uint32_t speed);
  
  bool setAccelRange(uint8_t range);
  bool setAccelDataRate(uint16_t rate);
  bool enableAccelAxes(bool x, bool y, bool z);
  bool setAccelPowerMode(LSM303AGR_PowerMode mode);
  bool setAccelEnabled(bool enabled);
  bool isAccelDataReady();
  bool readAccel(float &x, float &y, float &z);
  
  bool setMagMode(LSM303AGR_MagMode mode);
  bool setMagDataRate(uint16_t rate);
  bool setMagOffsetCancellation(bool enabled);
  bool isMagDataReady();
  bool readMag(float &x, float &y, float &z);
  
  bool enableTemperature(bool enabled);
  float readTemperature();
  
  bool runAccelSelfTest();
  bool runMagSelfTest();
  
  bool isI2C();
  bool isSPI();

private:
  TwoWire* _wire;
  SPIClass* _spi;
  uint8_t _csAccel;
  uint8_t _csMag;
  uint32_t _spiSpeed;
  LSM303AGR_Protocol _protocol;
  
  uint8_t _accelRange;
  float _accelScale;
  
  void writeAccelReg(uint8_t reg, uint8_t value);
  uint8_t readAccelReg(uint8_t reg);
  void writeMagReg(uint8_t reg, uint8_t value);
  uint8_t readMagReg(uint8_t reg);
  
  void writeAccelRegI2C(uint8_t reg, uint8_t value);
  uint8_t readAccelRegI2C(uint8_t reg);
  void writeMagRegI2C(uint8_t reg, uint8_t value);
  uint8_t readMagRegI2C(uint8_t reg);
  void readMultiI2C(uint8_t address, uint8_t reg, uint8_t* buffer, uint8_t length);
  
  void writeAccelRegSPI(uint8_t reg, uint8_t value);
  uint8_t readAccelRegSPI(uint8_t reg);
  void writeMagRegSPI(uint8_t reg, uint8_t value);
  uint8_t readMagRegSPI(uint8_t reg);
  void readMultiSPI(uint8_t cs, uint8_t reg, uint8_t* buffer, uint8_t length);
  
  void updateAccelScale();
};

#endif
