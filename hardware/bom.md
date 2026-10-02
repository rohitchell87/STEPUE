# STEPUE POC — Final Bill of Materials

This is the **final procurement BOM supplied for the STEPUE Basic POC** as of October 2026.
It is intentionally treated as the POC BOM, not as a permanent product BOM. Components,
quantities, vendors, and specifications may be revised in later hardware iterations.

> **Source of truth:** `STEPUE_POC_Final_BOM.xlsx` is the user-supplied BOM. This Markdown
table mirrors that sheet for convenient viewing on GitHub.

| # | Component | Qty. | Estimated unit cost |
|---:|---|---:|---:|
| 1 | ESP32 DevKit V1 (ESP32-WROOM-32) | 1 | INR 350–450 |
| 2 | MPU6050 IMU Module | 1 | INR 150–180 |
| 3 | ADS1115 16-bit 4-Channel ADC Module | 1 | INR 160–210 |
| 4 | Interlink FSR UX 402 Pressure Sensors | 4 | INR 190–220 |
| 5 | Precision Microdrives 306-109 — 6 mm × 12.2 mm, 3 V vibration motor | 2 | INR 300–500 |
| 6 | AO3400/AO3400A Logic-Level N-Channel MOSFET | 2 | INR 8–15 |
| 7 | 1N4148 / 1N4007 Flyback Diode | 2 | INR 1–3 |
| 8 | 3.7 V Li-Po Battery, 1000–1500 mAh | 1 | INR 250–350 |
| 9 | TP4056 Li-Po Charger Module with Protection | 1 | INR 35–60 |
| 10 | 3.3 V Buck-Boost Regulator Module, Li-Po Compatible | 1 | INR 120–250 |
| 11 | Power ON/OFF Switch | 1 | INR 10–20 |
| 12 | Helios Memory Foam Insole for Men — Fit Size 10 | 1 | INR 250–350 |
| 13 | Fitcozi Adjustable Padded Ankle Strap — 1 Pair | 1 | INR 350 |
| 14 | 5 V USB Wall Adapter LRIPL LR867, 5 V/2 A, 10 W, USB-A | 1 | INR 200–300 |
| 15 | 1 m basic USB-A to Type-C cable | 1 | INR 70–120 |

**Estimated total:** INR 1,944–2,748

## Important implementation note

The BOM contains the vibration-motor, MOSFET, and flyback-diode hardware because these are part
of the **final POC procurement list**. The repository's current firmware/dashboard documentation
still distinguishes between hardware that has been procured/planned for the POC and functionality
that has actually been demonstrated in the current software build.

The present dashboard POC demonstrates the ESP32 + MPU6050 + ADS1115 + FSR sensing and
heuristic state-classification path. Motor/tactile cueing should only be described as implemented
when the corresponding hardware and firmware have actually been integrated and tested.

## Future revisions

This BOM is deliberately versioned as a POC BOM. Changes to sensor count, actuator type,
power architecture, enclosure, wiring, or other components can be made later without treating
this version as the final product design.
