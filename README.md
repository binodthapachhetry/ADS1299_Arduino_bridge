# ADS1299-Arduino Driver Library

Light-weight C++/Arduino library for driving the **Texas Instruments ADS1299**
8-channel, 24-bit simultaneous-sampling ADC (commonly used for EEG /
biopotential acquisition).

---

## Features
* Pure register-level SPI implementation – no dependency on Arduino’s `SPI` class.
* Full support for ADS1299 system commands (WAKEUP, STANDBY, RESET, START, STOP).
* Register read/write helpers with human-readable Serial output.
* Real-time sample acquisition via `updateData()` (status + 8 channels).
* Example sketch `examples/example_1/example_1.ino` shows minimal usage.

---

## Hardware / Wiring

| ADS1299 Pin | Arduino Pin                  | Notes                                  |
|-------------|-----------------------------|----------------------------------------|
| `SCLK`      | `SCK`                       | Hardware SPI clock                     |
| `MOSI`      | `MOSI`                      | Hardware SPI MOSI                      |
| `MISO`      | `MISO`                      | Hardware SPI MISO                      |
| `CS`        | user-defined (`CS`)         | Chip-select, OUTPUT, active-LOW        |
| `DRDY`      | user-defined (`DRDY`)       | Data-ready interrupt, INPUT            |
| `RESET`     | optional                    | Tie to MCU RESET or drive via GPIO     |
| `START`     | optional                    | Pulled HIGH if not used                |
| `CLK`       | 2.048 MHz external crystal  |                                        |

> The library configures SPI at 1 MHz (16 MHz / 16) to satisfy ADS1299 timing.

---

## Installation

1. Copy the `ADS1299` folder (contains `ADS1299.h`, `ADS1299.cpp`,
   `Definitions.h`) into your Arduino `libraries` directory.
2. Restart the Arduino IDE.

---

## Quick Start

```cpp
#include <ADS1299.h>

ADS1299 ads;

const int DRDY_PIN = 9;    // ADS1299 DRDY
const int CS_PIN   = 10;   // ADS1299 CS

void setup() {
  Serial.begin(115200);
  ads.setup(DRDY_PIN, CS_PIN);

  ads.RESET();
  ads.WAKEUP();
  ads.SDATAC();                    // stop read-continuous while configuring

  // Example: CONFIG1 – 500 SPS, high-resolution
  ads.WREG(CONFIG1, 0b10010010);

  ads.RDATAC();                    // resume read-continuous
  ads.START();
}

void loop() {
  ads.updateData();                // prints one line per DRDY pulse
}
```

---

## Public API (summary)

| Function                 | Purpose                                           |
|--------------------------|---------------------------------------------------|
| `setup(DRDY, CS)`        | Initialise SPI and assign pin numbers             |
| `WAKEUP()` … `STOP()`    | Direct system commands                            |
| `RDATAC() / SDATAC()`    | Enable/disable read-continuous                    |
| `RDATA()`                | Single-shot data read                             |
| `RREG(addr[, n])`        | Read one / *n* consecutive registers              |
| `WREG(addr, val)`        | Write register                                    |
| `updateData()`           | When `DRDY` LOW, reads 27 bytes and prints hex    |
| `printRegisterName(addr)`| Helper: label for register address                |

---

## Timing

* `tCLK` constant = **666 ns** (datasheet p. 8).
* Library obeys required command spacing (`delay(4*tCLK)` etc.).

---

## Tested With

* **Arduino Uno R3** (16 MHz @ 5 V)  
* **ADS1299 Evaluation Board** rev A

---

## Contributing

Pull-requests welcome. Please run `clang-format` and include a minimal test
sketch.

---

## License

MIT © 2024 Oen for commercial and research use. Pull requests welcome.
