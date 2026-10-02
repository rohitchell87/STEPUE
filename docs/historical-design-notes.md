# Historical Design Notes

This file preserves ideas that appeared during STEPUE development but are intentionally separated from the current POC description.

## Earlier cueing concept

Earlier documentation explored a tactile-cue pathway in which a detected target condition could drive an actuator. Components discussed included vibrotactile motors, MOSFET switching and flyback protection.

## Why it is separated

The current POC report states that **no motor or tactile cue actuator is currently being used**. Therefore the repository should not present the earlier actuator concept as implemented hardware.

## Earlier broader hardware exploration

The project also explored:

- multiple FSR pressure sensors,
- vibration motors,
- MOSFET drivers,
- flyback diodes,
- microSD storage,
- portable battery/power-management components,
- ankle strap/enclosure concepts.

These belong to the development history and future integration path rather than the verified current dashboard POC.

### BOM clarification

The final POC procurement BOM now includes two 3 V vibration motors, two AO3400/AO3400A
MOSFETs and two flyback diodes. Their presence in the BOM should not be read as proof that
the current dashboard firmware has a working tactile-cue output. The implementation status is
documented separately from procurement scope.
