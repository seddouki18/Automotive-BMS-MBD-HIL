# Firmware review and reproduction status

This review covers the checked-in STM32 integration, controller C source, ESP32 plant, and initialization script. Existing screenshots and performance figures are historical project evidence; the changed firmware has not been rebuilt with STM32CubeIDE or tested on the hardware bench.

## Corrections in this change

- Execute the controller once per accepted CAN frame. Previously the callback called it twice, advancing Stateflow ticks and SOC smoothing twice per frame.
- Require standard data frames, identifier 0x100, DLC 5 before decoding or refreshing the watchdog.
- Keep PA5 low at startup until the controller authorizes relay closure.
- Zero-initialize the ESP32 TWAI message, including unused flags.
- Correct the UVP serial message to 2.20 V, matching the injected value.
- Describe the deployed 3–12–6–1 tansig network rather than the previously documented 3–16–8–1 ReLU network.

## Reproduction blockers

The current tree does not include main.h, Controller_BMS.h, Controller_BMS_private.h, or rtwtypes.h referenced by the application sources. Restore the original project headers or regenerate them from the matching model/toolchain; do not infer controller structures from C usage. The Python dashboard source is also absent. Use the ESP32 serial monitor for the interface currently available.

Import the STM32CubeIDE project only after restoring its headers. Record MATLAB, support-package, CubeIDE, and ESP32 core versions when reproducing the bench.

## Remaining integration work

| Finding | Consequence / next validation |
| --- | --- |
| INIT transitions to NORMAL after 10 steps without checking cell limits in that transition | Test startup with each active fault; move startup qualification into Stateflow before hardware use |
| Emergency-stop recovery is manually added to generated C | Healthy measurements can automatically reclose the relay; command 0 is sent only to the ESP32, not received as a reset command by STM32. Define recovery requirements in the model |
| Watchdog opens PA5 outside Stateflow | Model outputs can still report relay=true during CAN loss; test loss and recovery and consolidate actuator ownership |
| Main loop checks timeout then writes PA5 while CAN callback can preempt it | A fresh callback decision can be overwritten by a stale timeout decision; serialize relay updates in a future integration change |
| Model initialization script uses Ts=10 ms; HIL steps arrive every 100 ms | Reconcile temporal counters and per-step SOC smoothing with the deployed sample period |
| ESP32 advances WLTP index every 100 ms but prints it as seconds | Verify the sample interval of wltp_data.h before interpreting profile duration |
| DERATING is an output flag; HIL plant has no relay/derating feedback | Demonstrated state transitions do not establish actual load power reduction |
| DWT covers controller execution plus GPIO command | Re-measure after removing the duplicate step; neither DWT nor estimated relay delay proves physical shutdown time |

The initialization script creates a synthetic driving-current profile. Confirm the origin and sampling of each profile before describing it as a standard WLTP dataset. SOC figures from a nominal cycle do not establish generalization across cells, temperatures, aging, or out-of-distribution measurements.

## Host regression test

Run from the repository root:

```sh
python3 tests/test_can_callback.py
```

The test extracts the actual callback from main.c, compiles it with a minimal HAL/model stub, and executes assertions for one controller step per valid frame, decoded current and temperature, rejected short/extended/remote/wrong-ID frames, and failed HAL reads. A host C compiler is required. This checks integration logic only; it does not simulate STM32 peripherals, prove timing, or execute the neural-network controller.

## Bench checks after restoring the build

1. At power-up, verify PA5 remains low during INIT.
2. Send valid 0x100/DLC-5 frames; verify ten controller steps are needed after initial chart entry before INIT completes.
3. Inject commands 1–4 and verify physical contact opening as well as PA5.
4. Send malformed frames and stop valid traffic; verify they do not refresh the watchdog.
5. Disconnect CAN, then reconnect during nominal and faulted conditions; record recovery behavior.
6. Re-measure DWT and end-to-end contact switching. Keep nominal fault timing and communication-loss timing separate.
