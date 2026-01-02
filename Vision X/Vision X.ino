#include <Wire.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// 0.96" SPI OLED pinout:
// D0 (CLK/SCK) -> D13
// D1 (MOSI)    -> D11
// DC           -> D4
// CS           -> D5
// RES (RST)    -> D6
// VCC          -> 5V
// GND          -> GND
#define OLED_DC 4
#define OLED_CS 5
#define OLED_RST 6
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &SPI, OLED_DC, OLED_RST, OLED_CS);

// HC-05 Bluetooth on pins 8 (RX), 9 (TX) to avoid SPI pin conflicts
SoftwareSerial BTSerial(8, 9);

// Optional capture button (connect to GND when pressed)
const int BUTTON_PIN = 2;
const unsigned long DEBOUNCE_MS = 120;

struct DisplayState
{
    String objectName = "---";
    int confidence = -1; // -1 means unknown
    String message = "Waiting";
};

DisplayState state;
String incomingLine;
unsigned long lastButtonChange = 0;
bool lastButtonState = HIGH;

void renderDisplay()
{
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Compact header to leave more room for received text
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Vision X");

    // Larger fields for visibility
    display.setTextSize(1);
    display.setCursor(0, 10);
    display.print("O:");
    display.println(state.objectName);

    display.setCursor(0, 28);
    display.print("C:");
    if (state.confidence >= 0)
    {
        display.print(state.confidence);
        display.println("%");
    }
    else
    {
        display.println("--");
    }

    display.setCursor(0, 46);
    display.print("M:");
    display.println(state.message);

    display.display();
}

String trimmedValue(const String &raw)
{
    String v = raw;
    v.trim();
    return v;
}

void applyLine(const String &line)
{
    if (line.startsWith("OBJ:"))
    {
        state.objectName = trimmedValue(line.substring(4));
    }
    else if (line.startsWith("CONF:"))
    {
        String v = trimmedValue(line.substring(5));
        state.confidence = v.toInt();
    }
    else if (line.startsWith("MSG:"))
    {
        state.message = trimmedValue(line.substring(4));
    }
    else if (line.startsWith("CALL:"))
    {
        state.message = trimmedValue(line.substring(5));
    }
    else if (line == "RESET")
    {
        state = DisplayState();
    }
    else
    {
        // Fallback: treat any plain line as message text
        state.message = trimmedValue(line);
    }
    renderDisplay();
}

void setup()
{
    Serial.begin(9600);
    BTSerial.begin(9600);

    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // SPI OLED: no I2C address parameter
    if (!display.begin(SSD1306_SWITCHCAPVCC))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.println("Vision X");
    display.setTextSize(1);
    display.println("Ready");
    display.display();
}

void loop()
{
    // Read incoming HC-05 text lines terminated by '\n'
    while (BTSerial.available())
    {
        char c = BTSerial.read();
        if (c == '\n')
        {
            applyLine(incomingLine);
            incomingLine = "";
        }
        else if (c != '\r')
        {
            incomingLine += c;
        }
    }

    // Debounced button to request a scan
    bool reading = digitalRead(BUTTON_PIN);
    unsigned long now = millis();
    if (reading != lastButtonState && (now - lastButtonChange) > DEBOUNCE_MS)
    {
        lastButtonChange = now;
        lastButtonState = reading;
        if (reading == LOW)
        {
            BTSerial.println("SCAN");
            Serial.println("Button pressed: SCAN");
        }
    }
}