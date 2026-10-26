# CAN plant interface

[Overview](../README.md) · [Setup](GETTING_STARTED.md)

## Frame definition

| Property | Value |
| --- | --- |
| Producer | ESP32 plant |
| Consumer | STM32 controller |
| Bit rate | 500 kbit/s |
| Identifier | 0x100, standard 11-bit |
| Frame type | Data frame |
| DLC | Exactly 5 |
| Nominal interval | 100 ms |
| Byte order | Most-significant byte first |

| Bytes | Signal | Wire type | Conversion |
| --- | --- | --- | --- |
| 0–1 | Cell voltage | unsigned 16-bit | raw / 1000 V |
| 2–3 | Cell current | signed 16-bit, two's complement | raw / 100 A |
| 4 | Temperature | signed 8-bit, two's complement | raw °C |

Positive current represents discharge; negative current represents regeneration/charging.

## Examples

| Measurements | Payload in hexadecimal |
| --- | --- |
| 4.300 V, 35.00 A, 60 °C | 10 CC 0D AC 3C |
| 3.700 V, −2.00 A, 25 °C | 0E 74 FF 38 19 |
| 2.200 V, 0.00 A, 25 °C | 08 98 00 00 19 |

The ESP32 casts scaled values to integer types; temperature is truncated to whole degrees. Encoding limits are not physical plausibility checks, and out-of-range sensor values require a separate policy before conversion.

## Consumer behavior

Only matching standard data frames with DLC 5 refresh the receive timestamp or advance the controller. A valid frame triggers one controller step and one relay command. Failed HAL reads and other frame shapes are ignored.

The main loop commands PA5 low when the unsigned tick difference exceeds 300 ms; its 10 ms delay adds polling latency. This timeout is separate from Stateflow and has no dedicated fault-code output. Communication recovery and relay ownership remain integration items described in the [firmware review](FIRMWARE_REVIEW.md).

There is no sequence counter, application CRC, acknowledgement/status frame, or relay feedback in this protocol. No end-to-end application protection is claimed.
