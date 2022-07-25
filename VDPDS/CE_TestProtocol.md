# VD PDS Cold Electronics Test Protocol

## Motherboard test

See schematic ```DCEM*```.

Two SiPM electric signal amplifiers. First amplifier has gain of ~10. The 2nd amplifier converts differential input to single output and feed to laser driver which doesn't take differential input.

Tests (warm + cold):
1. 5V LDO output (requires probe wires to access in dewar)
2. Laser diode power (power meter) (2 channels)
3. Voltage across laser diode driver bias resistor (requires probe wires to access in dewar) (2 channels)
4. Voltage at op amp + input on laser diode driver circuit (requires probe wires to access in dewar) (2 channels)
5. Laser diode baseline on Koheron/scope (2 channels) (characterize noise)
6. PICO output (these will be different on the two boards because of different R3 values on B_S boards (requires probe wires to access in dewar) (2 channels)
7. Measure power drawn by 6 V supply (voltage and current)

Tools needed:
1. power supply
2. power meter
3. multimeter
4. oscilloscope


## LDO test

See schematic ```DCEM Power over Fiber``` right-most part.

The LDO regulates the input power supply voltage (6V) to 5V (or below). This 5V is then supplied to several places:

```
Two laser diode adapters (see schematic: Active Laser Driver)
Two SiPM electric signal amplifiers (see schematic: DCEM Amplifer CH1/2)
DC-DC SODIMM bias board
```

Check the LDO output pin 3 and 4 to see if 5V is output.

## DC-DC converter bias step-up test

See schematic ```DUNE BIAS SODIMM```.

Check the bias step up from 3.3V (or 5V?) to 56V (46V) in warm (cold) with both multimeter and oscilloscope (check baseline noise). Ideally from Mike (Iowa) noise should be ~1mV (single PE is ~2.5-5mV).

## Laser diode driver

See schematic ```Active Laser Driver```.

This is on the Argon2 board in the middle of the motherboard. When powered on after the LDO, the photodiode beside the laser diode driver should see some light, check with power meter (on the level of -5dBm? Confirm with Alan).

## SiPM test

Check SiPM I-V curve (with SSP if available at CERN) in warm and cold.

Check applying bias (56V) to SiPM (how many flex circuits?) with calibration light, it can output sensible signal on oscilloscope. Signal should be stable and not noisy. What intensity of the calibration light should we use?

## Fibers

We generally use multimode (50 microns) or single mode (9 micron) fibers for SoF, laser diode driver converts electric signal to optical signal and transmit via fibers in LAr. The optical signal then goes to optical receivers which converts the optical signal to electric signal for readout/monitor.

## Connectors

## Light calibration system

Is the light calibration system at CERN same as PAB? (commercial black device or the Zelimir's custom LED board).

# Troubleshoot
If don't see signals after calibration light is sent
