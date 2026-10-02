# STEPUE — Architecture

## High-level architecture

```mermaid
flowchart TB
    subgraph Sensing
        IMU[MPU6050\nAccelerometer + Gyroscope]
        FSR[FSR Pressure Sensor]
        ADC[ADS1115\n16-bit ADC]
        FSR --> ADC
    end

    subgraph Processing
        ESP[ESP32]
        FE[Movement + Gyro Feature Calculation]
        SM[Heuristic State Machine]
        ADC --> ESP
        IMU --> ESP
        ESP --> FE --> SM
    end

    subgraph Communication
        WIFI[Local Wi-Fi]
        DASH[ESP32-hosted Web Dashboard]
        SM --> WIFI --> DASH
        ADC --> DASH
        FE --> DASH
    end

    POWER[Li-Po + Charging/Regulation Path] --> ESP
```

## I²C bus

The MPU6050 and ADS1115 share the ESP32 I²C bus:

```text
ESP32 GPIO21 SDA ──┬── MPU6050 SDA
                   └── ADS1115 SDA

ESP32 GPIO22 SCL ──┬── MPU6050 SCL
                   └── ADS1115 SCL
```

## Data path

```text
Raw IMU data
    ↓
Acceleration magnitude + gyro magnitude
    ↓
Movement / disturbance heuristics
    ↓
State machine
    ↓
STANDING / WALKING / FOG-LIKE CONDITION
```

In parallel:

```text
FSR
 ↓
ADS1115 A0
 ↓
Raw ADC + voltage
 ↓
NO / LOW / MEDIUM / HIGH PRESSURE
 ↓
Dashboard
```
