/*
 * Renogy Wanderer Charge Current bar graph
 * MIT License
 *
 * Arduino Nano or similar
 *
 * Connections:
 *   Soft serial to the Modbus RS232 port. Rx pin 6, Tx pin 7
 *   On-board LED for heartbeat
 *   8-led WS2812 strip on pin 9
 * 
 * Libraries required:
 *  SoftwareSerial  (install with Arduino IDE)
 *  Modbus Master   (https://docs.arduino.cc/libraries/modbusmaster/)
 *  FastLED         (https://github.com/FastLED/FastLED/releases/tag/3.10.2)
 *
 * Greatly assisted by:
 *   Pin out: https://solarthing.readthedocs.io/en/latest/quickstart/serial-port/hardware/rover/rs232-port.html
 *   Modbus address and registers: https://github.com/BarkinSpider/SolarShed/blob/master/RenogyWanderer.py
 *
 */

#include <ModbusMaster.h>
#include <FastLED.h>
#include <SoftwareSerial.h>
#define NUM_LEDS 8
#define DATA_PIN 9

CRGB leds[NUM_LEDS];

const int ledPin = 13;       // white LED
const int RX_PIN = 6;
const int TX_PIN = 7;
const int MODBUS_DEVICE_ADDRESS = 255;
const int TARGET_REGISTER_ADDRESS = 0x108; //    PVamps*100 = 0x108   BatteryVolt*10 = 0x101


// Create a SoftwareSerial instance to communicate with the Modbus device.
SoftwareSerial mySerial(RX_PIN, TX_PIN);

// Create a ModbusMaster instance.
ModbusMaster node;


void setup()  
{
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  Serial.println("Renogy bar graph monitor");

  pinMode(ledPin, OUTPUT);
   
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // Start the software serial port for Modbus communication.
  // IMPORTANT: Make sure this baud rate matches your Modbus device's configuration.
  mySerial.begin(9600);

  // Initialize the ModbusMaster object.
  // Pass the device address and the serial port instance.
  node.begin(MODBUS_DEVICE_ADDRESS, mySerial);

}



void loop() {
  uint8_t result;
  uint16_t registerValue;  

  digitalWrite(ledPin,1);

  result = node.readHoldingRegisters(TARGET_REGISTER_ADDRESS, 1);

  // Check if the request was successful.
  if (result == node.ku8MBSuccess) {

    // The Modbus protocol is big-endian.  getResponseBuffer(0) retrieves the first 16-bit register value.
    registerValue = node.getResponseBuffer(0);

    Serial.print("Value (unsigned decimal): ");
    Serial.println(registerValue);

    // scale and display the value of current
    const int INPUT_MIN = 0;   //115;
    const int INPUT_MAX = 600; //145;
    const int HUE = 90;        // Green
    const int MAX_BRIGHTNESS = 100; // Max brightness for a lit LED
    const int MIN_BRIGHTNESS = 0;   // Brightness for an unlit LED

    float value = constrain(registerValue, INPUT_MIN, INPUT_MAX);
    float mappedValue = (value - INPUT_MIN) / (INPUT_MAX - INPUT_MIN) * NUM_LEDS;
    int numFullLeds = floor(mappedValue);
    float tipFraction = mappedValue - numFullLeds;
    uint8_t tipBrightness = int(tipFraction * (MAX_BRIGHTNESS-MIN_BRIGHTNESS))+MIN_BRIGHTNESS;
    for (int i = 0; i < NUM_LEDS; i++) {
      if (i < numFullLeds) {
        // These LEDs are fully on.
        leds[i].setHSV(HUE, 255, MAX_BRIGHTNESS);
      } else if (i == numFullLeds) {
        // This is the anti-aliased "tip" of the bar graph.
        leds[i].setHSV(HUE, 255, tipBrightness);
      } else {
        // These LEDs are off (or very dim).
        leds[i].setHSV(HUE, 255, MIN_BRIGHTNESS);
      }
    }
    if ((tipBrightness<2)&&(numFullLeds==0)) {
      leds[0].setHSV(180, 255, 2);
    }
    FastLED.show();

  } else {
    // If the request failed, print an error code for diagnostics.
    Serial.print("  Failed to read register. Error code: 0x");
    Serial.print(result, HEX);

    // Provide a human-readable explanation for common errors
    if (result == node.ku8MBInvalidSlaveID) Serial.println(" (Invalid Slave ID - Check address)");
    else if (result == node.ku8MBInvalidFunction) Serial.println(" (Invalid Function Code)");
    else if (result == node.ku8MBIllegalDataAddress) Serial.println(" (Invalid Data Address - Register doesn't exist)");
    else if (result == node.ku8MBResponseTimedOut) Serial.println(" (Response Timed Out - Check wiring, baud rate, and device power)");
    else Serial.println(" (Unknown Error)");

    // show error on the display. All LEDs blank, then first LED red.
    for (int a=0; a<NUM_LEDS; a++) {
      leds[a].setHSV(0, 255, 0);   // hue  sat  bri
    }
    leds[0].setHSV(0, 255, 10);
    FastLED.show();
    
  }

  digitalWrite(ledPin,0);

  // the Wanderer updates a few times a second, but the values are smoothed over about 2 seconds, so this is quite fast enough
  delay(200);
  
}
