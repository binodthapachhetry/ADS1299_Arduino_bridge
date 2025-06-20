/**
 * Example: stream_eeg.ino
 * Streams raw ADS1299 data over Serial (115200 baud).
 * Hardware wiring – see README “Hardware / Wiring”.
 */

#include <ADS1299.h>
ADS1299 ads;

const uint8_t DRDY_PIN = 9;   // ADS1299 → Arduino
const uint8_t CS_PIN   = 10;

void setup() {
  Serial.begin(115200);

  ads.setup(DRDY_PIN, CS_PIN);

  /* --- power-up & config --- */
  ads.RESET();
  ads.WAKEUP();
  ads.SDATAC();                      // halt continuous read while we program

  ads.WREG(CONFIG1, 0b10000110);     // HR mode, 250 SPS
  ads.WREG(CONFIG2, 0b11000000);     // internal reference, test off
  ads.WREG(CONFIG3, 0b01101100);     // bias buffer enabled

  for (byte ch = CH1SET; ch <= CH8SET; ch++)
    ads.WREG(ch, 0b01100000);        // gain = 24, channel ON

  ads.RDATAC();                      // resume continuous
  ads.START();                       // begin conversions
}

void loop() {
  ads.updateData();                  // prints one sample frame when DRDY low
}
