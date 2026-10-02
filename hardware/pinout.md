# STEPUE POC — Pinout & Wiring

## ESP32 ↔ MPU6050

| MPU6050 | ESP32 |
|---|---|
| VCC | 3.3 V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

I²C address used by the firmware: `0x68`.

## ESP32 ↔ ADS1115

| ADS1115 | ESP32 |
|---|---|
| VDD | 3.3 V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

I²C address used by the firmware: `0x49`.

## FSR ↔ ADS1115

The current documented pressure path is:

```text
FSR → ADS1115 A0 → ESP32 over I²C
```

The firmware reads A0 using `readADC_SingleEnded(0)` and converts the result using `computeVolts()`.

The exact resistor value used in the physical FSR voltage-divider wiring is not preserved in the current documentation, so this repository intentionally does not invent one. Record it here once the physical wiring is finalized.

## Power path

The current POC has been developed primarily with the ESP32 powered/programmed through USB. A Li-Po + TP4056 + boost/regulation path is being tested for portable operation.

```text
Li-Po battery
     ↓
TP4056 / protection
     ↓
Boost / regulation
     ↓
ESP32 + sensors
```

Battery-only boot and reliable Wi-Fi operation remain under testing.
