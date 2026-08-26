# Boardoza LSM303 Triple Axis Accelerometer & Magnetometer Breakout Board

The **Boardoza LSM303 Breakout Board** is a compact and ultra-low-power motion sensing solution featuring the **STMicroelectronics LSM303AGR** 6-axis e-compass sensor. It combines a **3-axis digital accelerometer** and a **3-axis digital magnetometer** in a single package, enabling accurate motion tracking, orientation sensing, and tilt-compensated compass applications.

Designed for seamless integration with both modern and legacy development platforms, the board includes a **TXS0104 bi-directional logic level shifter**, allowing direct compatibility with both **3.3V and 5V microcontrollers** such as ESP32, STM32, Raspberry Pi Pico, and Arduino Uno. With support for high-speed I<sup>2</sup>C communication and advanced interrupt functionality, this board is ideal for **robotics, wearable devices, navigation systems, drones, IoT applications, and smart motion-based projects**.


| Front Side | Back Side |
|:---:|:---:|
| ![LSM303AGR Front](./assets/LSM303%20Front.png) | ![LSM303AGR Back](./assets/LSM303%20Back.png) |

---

## Key Features

- **Integrated 6-Axis Motion Sensor:** Combines a 3-axis accelerometer and 3-axis magnetometer in a single compact module.
- **True E-Compass Functionality:** Enables accurate heading and orientation measurements using tilt compensation.
- **Wide Logic Compatibility:** Built-in TXS0104 level shifter supports both 3.3V and 5V systems.
- **High-Speed I<sup>2</sup>C Communication:** Supports Standard, Fast Mode (400kHz), and Fast Mode Plus (1MHz).
- **Ultra-Low Power Operation:** Optimized for battery-powered and energy-efficient embedded systems.
- **Programmable Interrupts:** Supports free-fall detection, orientation sensing, magnetic threshold detection, and motion events.

---

## Technical Specifications

**Model:** LSM303AGRTR  
**Manufacturer:** Boardoza    
**Manufacturer IC:** STMicroelectronics   
**Sensor Type:** 3-Axis Accelerometer + 3-Axis Magnetometer  
**Input Voltage:** 3.3 VDC - 5V DC  
**Voltage Logic Compatibility:** 3.3V / 5V (via TXS0104 Level Shifter)  
**Communication Interface:** I<sup>2</sup>C  / SPI  
**I<sup>2</sup>C Speed:** Standard Mode (100kHz), Fast Mode (400kHz), Fast Mode Plus (1MHz)   
**Accelerometer Full Scale Range:** ±2g / ±4g / ±8g / ±16g  
**Magnetic Field Range:** ±50 gauss  
**Accelerometer Resolution:** 12-bit  
**Magnetometer Resolution:** 16-bit  
**Functions:** Motion Detection, Orientation Sensing, Digital Compass, Tilt Compensation  
**Interrupt Features:** Free-fall, 6D/4D orientation, magnetic threshold interrupt  
**Operating Temperature:** -40°C to +85°C  
**Board Dimensions:** 20mm x 20mm  

---

## Board Pinout

### ( J1 ) Main Communication Connector

| Pin Number | Pin Name | Description |
|:---:|:---:|---|
| 1 | VCC | Voltage Supply Input |
| 2 | SDA / SDI / SDO | I<sup>2</sup>C Data / SPI Data Input / 3-wire SPI Data Output |
| 3 | /CS_MAG | Active-Low Magnetometer SPI Chip Select |
| 4 | /CS_XL | Active-Low Accelerometer SPI Chip Select |
| 5 | SCL / SPC | I<sup>2</sup>C Clock / SPI Clock |
| 6 | GND | Ground |

### ( J2 ) Interrupt Connector

| Pin Number | Pin Name | Description |
|:---:|:---:|---|
| 1 | XL1 | Accelerometer Interrupt Output 1 (3.3V Logic) |
| 2 | XL2 | Accelerometer Interrupt Output 2 (3.3V Logic) |
| 3 | MAG | Magnetometer Interrupt / Data Ready Output (3.3V Logic) |

---

## Board Dimensions

<img src="./assets/LSM303 Dimensions.png" alt="LSM303AGRTR Dimensions" width="450"/>

---

## Step Files

[Boardoza LSM303AGRTR.step](./assets/LSM303%20Step.step)

---

## Datasheet

[LSM303AGR Datasheet.pdf](./assets/LSM303%20Datasheet.pdf)

---

## Version History

- V1.0.0 - Initial Release

---

## Support

- If you have any questions or need support, please contact support@boardoza.com

---

## **License**

This repository contains both hardware and software components:

### **Hardware Design**

[![CC BY-SA 4.0][cc-by-sa-shield]][cc-by-sa]

All hardware design files are licensed under [Creative Commons Attribution-ShareAlike 4.0 International License][cc-by-sa].

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg

### **Software/Firmware**

[![BSD-3-Clause][bsd-shield]][bsd]

All software and firmware are licensed under [BSD 3-Clause License][bsd].

[bsd]: https://opensource.org/licenses/BSD-3-Clause
[bsd-shield]: https://img.shields.io/badge/License-BSD%203--Clause-blue.svg
