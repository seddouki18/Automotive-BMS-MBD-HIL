# Getting started

[Overview](../README.md) · [CAN interface](CAN_PROTOCOL.md) · [Firmware review](FIRMWARE_REVIEW.md)

## Choose a workflow

| Workflow | Requirements | Current status |
| --- | --- | --- |
| Host regression | Python 3, C compiler available as cc | Runnable without MATLAB or boards |
| Model exploration / MIL | MATLAB, Simulink, Stateflow and model dependencies | Local MATLAB validation required |
| Code generation / SIL / PIL | Embedded Coder, target support package, matching model artifacts, STM32 tools | Revision/toolchain reconciliation required |
| HIL bench | ESP32, NUCLEO-L476RG, two CAN transceivers, relay, matching build | Missing STM32 headers must first be restored |

## 1. Obtain the sources

```sh
git clone --branch main https://github.com/seddouki18/Automotive-BMS-MBD-HIL.git
cd Automotive-BMS-MBD-HIL
python3 tests/test_can_callback.py
python3 tests/check_docs.py
```

The repository includes large demonstration videos; cloning may take time.

## 2. Explore the model

Open MATLAB in `Model_Based_Design_Simulink/`, inspect `init_bms.m`, then run it and open `BMS_controller.slx`. The script sets `Ts = 0.01` and `sim_time = 1800`, and creates a synthetic current timeseries.

The initialization script optionally loads `reseau_poids.mat`, which is not included under that name. Other network artifacts exist, but must not be substituted without checking their structure and the model callbacks. See the [technical report](TECHNICAL_REPORT.md) for the historical simulation workflow.

Before regenerating controller C, match the training/inference scripts, network weights, model revision, and expected sample time. Record tool versions with each new result.

## 3. Restore and build STM32 firmware

Import `Firmware_STM32_Controller/BMS_STM32_HIL` into STM32CubeIDE. Restore original application/generated headers, including `main.h`, `Controller_BMS.h`, `Controller_BMS_private.h`, and `rtwtypes.h`; verify include paths and all dependencies before building.

The pre-existing Debug binaries are historical artifacts and do not include the fixes in this branch. Rebuild from sources and flash through ST-LINK only after the build succeeds.

## 4. Prepare the ESP32 plant

Open `Firmware_ESP32_Plant/BMS_Plant_ESP32.ino` with `wltp_data.h` available. Select the matching ESP32 board/core in Arduino IDE, compile, and upload. The firmware uses the ESP32 TWAI driver.

| Signal | ESP32 | STM32 |
| --- | --- | --- |
| CAN controller TX → transceiver TXD | GPIO25 | PA12 |
| CAN controller RX ← transceiver RXD | GPIO26 | PA11 |
| Relay command | — | PA5 |
| Serial monitor | USB, 115200 baud | ST-LINK/debug workflow |

Connect each CAN controller through its own SN65HVD230. Join CANH to CANH, CANL to CANL, and grounds; use bus termination appropriate to the two endpoints (normally 120 Ω at each end), accounting for any resistors already fitted to the modules. Verify the relay module polarity against PA5 and actual contact state.

## 5. Exercise the bench

Open the ESP32 serial monitor at 115200 baud.

| Command | Scenario |
| --- | --- |
| 0 | Remove fault override; resume plant evolution |
| 1 | Voltage override: 4.30 V |
| 2 | Temperature override: 60 °C |
| 3 | Voltage override: 2.20 V |
| 4 | Discharge current override: 35 A |

Command 0 does not reset accumulated cell states and is not an explicit reset command to the STM32. Temperature can remain elevated after an OTP injection.

Observe inputs, state, outputs, and PA5 in STM32CubeIDE. Confirm relay contact opening separately from an LED indication. Follow the [review checklist](FIRMWARE_REVIEW.md) before reporting new PASS results. The demonstration dashboard source is not currently supplied.

## Troubleshooting

| Symptom | Check |
| --- | --- |
| STM32 include errors | Restore original/generated headers; check project include paths |
| No CAN reception | Common ground, CANH/CANL, termination, 500 kbit/s, TX/RX wiring and IRQ handler |
| Relay stays open | Valid 0x100 standard data frames, DLC 5, INIT progression, cell limits and 300 ms watchdog |
| Unexpected recovery | Review manually added emergency-stop recovery conditions |
| Plots disagree with firmware | Match network revision, sample time and profile source before comparing results |
