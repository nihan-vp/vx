# Vision X Glasses (Nano/UNO + HC-05 + 0.96" SPI OLED)

## What this sketch does
- Listens to the HC-05 on pins 8 (RX) / 9 (TX) for newline-terminated text.
- Renders OBJ / CONF / MSG fields on a 128x64 0.96" SPI OLED (Adafruit_SSD1306 SPI).
- Debounced button on D2 sends `SCAN` back over Bluetooth.

## Pinout (Arduino Nano/UNO)

### SPI OLED (0.96" 128x64)
- OLED D0 (CLK/SCK) → D13
- OLED D1 (MOSI)    → D11
- OLED DC           → D4
- OLED CS           → D5
- OLED RST (RES)    → D6
- OLED VCC          → 5V
- OLED GND          → GND

### I2C OLED (alternative wiring if you swap libraries/sketch)
- SDA → A4
- SCL → A5
- VCC → 5V
- GND → GND

### Bluetooth HC-05 (SoftwareSerial)
- HC-05 TX → D8 (Nano/UNO RX for SoftwareSerial)
- HC-05 RX → D9 (Nano/UNO TX for SoftwareSerial) **Use a resistor divider or level shifter; Nano/UNO is 5V, HC-05 expects 3.3V logic**
- HC-05 VCC → 5V (typical modules accept 5V on VCC)
- HC-05 GND → GND (common ground with everything)

### Capture Button
- One side → D2
- Other side → GND
- Internal pull-up is enabled in code.

## Expected serial/Bluetooth protocol
- Incoming lines (newline-terminated):
	- `OBJ:<name>`
	- `CONF:<0-100>`
	- `MSG:<text>`
	- `RESET`
- Outgoing when button pressed: `SCAN`\n
- The ESP32-CAM (if used for object detection) can send labels/confidence as short lines using the above keys.

## Power notes
- Share common ground across Nano/UNO, HC-05, OLED, and ESP32-CAM.
- If the ESP32-CAM is powered from 5V, keep its UART separate from the HC-05 pins above to avoid contention.

## Upload
- Board: "Arduino Nano" (select the correct processor/bootloader for your clone) or "Arduino/Genuino Uno" if on Uno.
- Upload speed: 57600 or 115200 depending on your bootloader.
- Library deps: Adafruit_GFX, Adafruit_SSD1306 (SPI build), SPI, SoftwareSerial.
