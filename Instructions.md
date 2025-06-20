# ADS1299 EEG Streaming – Quick-Start Instructions  
These steps take you from bare hardware to continuously logging ADS1299
EEG frames on your desktop OS (Windows / macOS / Linux).

---

## 1. Hardware Checklist
| Item | Qty | Notes |
|------|-----|-------|
| Arduino-compatible MCU | 1 | 5 V, 16 MHz (e.g. UNO R3) |
| TI ADS1299 board       | 1 | Eval board or custom PCB |
| 2.048 MHz crystal      | 1 | ADS1299 system clock |
| Active electrodes & leads | up to 8 | Refer to safety guidelines |
| Jumper wires           | — | see wiring table in `README.md` |

*Power rails:* 5 V for Arduino, 3.3 V or 5 V for ADS1299 analog/digital
per board spec.  
*Ground:* COMMON between ADS1299 and Arduino.

---

## 2. Software Prerequisites
1. **Arduino IDE** ≥ 1.8.x **OR** `arduino-cli` ≥ 0.33  
   Download: <https://www.arduino.cc/en/software>
2. Python ≥ 3.7 (for optional logging script)  
   `pip install pyserial`

> Linux only: add your user to the `dialout` (or `uucp`) group and
> reconnect USB so the serial port is accessible without `sudo`.

---

## 3. Library Installation
```text
<Arduino sketchbook>/
└── libraries/
    └── ADS1299/
        ├── ADS1299.h
        ├── ADS1299.cpp
        └── Definitions.h
```
Copy the `ADS1299` folder into `libraries/` and restart the IDE/CLI.

---

## 4. Flash the Streaming Sketch
Open **examples/stream_eeg/stream_eeg.ino** and click ▸ **Upload**  
(or via CLI):

```bash
arduino-cli compile --fqbn arduino:avr:uno examples/stream_eeg
arduino-cli upload  --fqbn arduino:avr:uno -p <serial-port> examples/stream_eeg
```

The board now outputs one sample frame per DRDY pulse (≈ 250 SPS by
default) at **115 200 baud**:

```
0, 0xC0FFFF, 0xFFFE91, 0x00027D, … 0xFFFABC
1, 0xC0FFFE, 0xFFFE8F, 0x000280, … 0xFFFAB3
```
Index, 24-bit STATUS, CH1–CH8 (hex, two’s-complement).

---

## 5. Continuous Logging on the PC

### 5.1 Quick terminal view  
IDE → Tools → **Serial Monitor**  
Select *115200 baud*.

### 5.2 Background CSV logger (Python)
Save as `log_eeg.py`:

```python
import csv, time, serial, datetime
ser = serial.Serial('/dev/ttyACM0', 115200, timeout=1)
ts = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
with open(f'eeg_{ts}.csv', 'w', newline='') as f:
    w = csv.writer(f)
    while True:
        line = ser.readline().decode('ascii','ignore').strip()
        if line:
            # split by comma + strip spaces
            w.writerow([time.time()] + [x.strip() for x in line.split(',')])
```

Run head-less:

```bash
nohup python3 log_eeg.py &
```

Result: `eeg_YYYYMMDD_HHMMSS.csv` grows continuously until you stop the
script (`kill %<job>` or `pkill -f log_eeg.py`).

---

## 6. Data Conversion to µV
Each 24-bit channel word (`signed long`) →  
`µV = word * ( Vref / gain / 2^23 )`

Default sketch parameters:  
* `Vref = 4.5 V` (internal reference)  
* `gain = 24`  
* scale ≈ **22.32 nV / LSB**

---

## 7. Running at Boot (optional, Linux)
Create `/etc/systemd/system/ads1299-logger.service`

```ini
[Unit]
Description=ADS1299 EEG Logger
After=network.target

[Service]
ExecStart=/usr/bin/python3 /home/<user>/log_eeg.py
Restart=always
User=<user>
Group=<user>

[Install]
WantedBy=multi-user.target
```

```bash
sudo systemctl enable ads1299-logger
sudo systemctl start  ads1299-logger
```

---

## 8. Troubleshooting
* **No serial output** – verify wiring, DRDY toggles, correct port.
* **Garbled data** – check baud rate; use shielded USB cable; ensure
  ground reference between boards.
* **Baseline drift / noise** – enable bias-drive (CONFIG3), shorten
  electrode leads, separate analog/digital grounds with 0 Ω link.

---

Happy hacking & safe EEG!
