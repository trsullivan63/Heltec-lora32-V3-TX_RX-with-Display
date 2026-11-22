#include "LoRaWan_APP.h"
#include "Arduino.h"
#include "DYPlayerArduino.h"
#include "pins_arduino.h"
#include <Wire.h>
//#include <Adafruit_SSD1306.h>
//#include <heltec_unofficial.h>
#include "HT_SSD1306Wire.h"
//#include "SSD1306Wire.h"
#include <Adafruit_NeoPixel.h>
int BlueLED = 45;
int RedLED = 46;
int GreenLED = 42;
int YellowLED = 41;
int demoMode = 0;
int counter = 1;
//int HT_SSD1306Wire.h

#define LED_PIN    48  // Onboard RGB LED pin on ESP32-S3 DevKitC-1
#define LED_COUNT 26   // Number of onboard NeoPixels

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

DY::Player player;

// addr , freq , i2c group , resolution , rst
SSD1306Wire factory_display(0x3c, 500000, SDA_OLED, SCL_OLED, GEOMETRY_128_64, RST_OLED);  // addr , freq , i2c group , resolution , rst
#define RF_FREQUENCY 915000000                                                             // Hz

#define TX_OUTPUT_POWER 14  // dBm

#define LORA_BANDWIDTH 0         // [0: 125 kHz, \
                                 //  1: 250 kHz, \
                                 //  2: 500 kHz, \
                                 //  3: Reserved]
#define LORA_SPREADING_FACTOR 7  // [SF7..SF12]
#define LORA_CODINGRATE 1        // [1: 4/5, \
                                 //  2: 4/6, \
                                 //  3: 4/7, \
                                 //  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8   // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0    // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false


#define RX_TIMEOUT_VALUE 1000
#define BUFFER_SIZE 30  // Define the payload size here

//#define DEMO_DURATION 3000  //ts not needed
//typedef void (*Demo)(void);

char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
//String rxpacket;

static RadioEvents_t RadioEvents;

int16_t txNumber;

int16_t rssi, rxSize;

bool lora_idle = true;

void VextON(void) {
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, LOW);
}
void VextOFF(void)  //Vext default OFF
{
  pinMode(Vext, OUTPUT);
  digitalWrite(Vext, HIGH);
}
void displayReset(void) {
  // Send a reset
  pinMode(RST_OLED, OUTPUT);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
  digitalWrite(RST_OLED, LOW);
  delay(1);
  digitalWrite(RST_OLED, HIGH);
  delay(1);
}
void setup() {
  Serial.begin(115200);

  player.begin();
  player.setVolume(20); // 50% Volume

  Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);
  factory_display.init();
  factory_display.clear();
  factory_display.display();

  while (!Serial) {
    delay(100);
  }
  Serial.println();
  Serial.println();
  // This turns on and resets the OLED on the Heltec boards
  VextON();
  displayReset();

  // Initialising the UI will init the display too.
  factory_display.init();
  factory_display.flipScreenVertically();
  factory_display.setFont(ArialMT_Plain_10);







  //edit
  pinMode(BlueLED, OUTPUT);
  digitalWrite(BlueLED, HIGH);
  pinMode(RedLED, OUTPUT);
  digitalWrite(RedLED, HIGH);
  pinMode(GreenLED, OUTPUT);
  digitalWrite(GreenLED, HIGH);
  pinMode(YellowLED, OUTPUT);
  digitalWrite(YellowLED, HIGH);
  txNumber = 0;
  rssi = 0;

  RadioEvents.RxDone = OnRxDone;
  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetRxConfig(MODEM_LORA, LORA_BANDWIDTH, LORA_SPREADING_FACTOR,
                    LORA_CODINGRATE, 0, LORA_PREAMBLE_LENGTH,
                    LORA_SYMBOL_TIMEOUT, LORA_FIX_LENGTH_PAYLOAD_ON,
                    0, true, 0, 0, LORA_IQ_INVERSION_ON, true);
}

void drawFontFaceDemo() {
  // Font Demo1
  // create more fonts at http://oleddisplay.squix.ch/
  factory_display.setTextAlignment(TEXT_ALIGN_CENTER);
  //factory_display.setFont(ArialMT_Plain_10);
  //factory_display.drawString(0, 0, "Hello world");
  //factory_display.setFont(ArialMT_Plain_16);
  //factory_display.drawString(0, 10, "Hello world");
  factory_display.setFont(ArialMT_Plain_24);
  factory_display.drawString(0, 24, "Hello world");
}


void loop() {
  if (lora_idle) {
    lora_idle = false;
    Serial.println("into RX mode");
    Radio.Rx(0);
  }
  Radio.IrqProcess();
}

void OnRxDone(uint8_t *payload, uint16_t size, int16_t rssi, int8_t snr) {
  rssi = rssi;
  rxSize = size;
  memcpy(rxpacket, payload, size);
  rxpacket[size] = '\0';
  Radio.Sleep();
  Serial.printf("\r\nreceived packet \"%s\" with rssi %d , length %d\r\n", rxpacket, rssi, rxSize);
  lora_idle = true;
  //edit

  if (String(rxpacket) == "BlueLED") {
    //digitalWrite(BlueLED, HIGH);
    factory_display.clear();
    factory_display.display();
    factory_display.setFont(ArialMT_Plain_24);
    factory_display.setTextAlignment(TEXT_ALIGN_LEFT);
    factory_display.drawString(0, 10, "  Blue LED");
    factory_display.display();

    player.playSpecified(1);
    //delay(5000);
   
    digitalWrite(BlueLED, LOW);
    
    delay(2000);
    
    digitalWrite(BlueLED, HIGH);
    //delay(200);
    //digitalWrite(BlueLED, LOW);
    //delay(2000);
    factory_display.clear();
    //digitalWrite(BlueLED, LOW);//test
    Serial.println(String(rxpacket));
    
    colorWipe(strip.Color(255, 0, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 255), 10);
    delay(5);// Blue
    //colorWipe(strip.Color(0, 0, 0), 10);
    colorWipe(strip.Color(255, 0, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 255), 10);
    delay(5);// Blue
    //colorWipe(strip.Color(0, 0, 0), 10);
    colorWipe(strip.Color(255, 0, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 255), 10);
    delay(5);// Blue
    colorWipe(strip.Color(0, 0, 0), 10);

    delay(5000);
    
  }
  if (String(rxpacket) == "RedLED") {
    digitalWrite(RedLED, LOW);
    digitalWrite(GreenLED, LOW);
    digitalWrite(BlueLED, LOW);
    delay(100);
    digitalWrite(GreenLED, HIGH);
    digitalWrite(BlueLED, HIGH);
  
    player.playSpecified(3);
    //delay(5000);

    factory_display.clear();
    factory_display.display();
    factory_display.setFont(ArialMT_Plain_24);
    factory_display.setTextAlignment(TEXT_ALIGN_LEFT);
    factory_display.drawString(0, 10, "  Red LED");
    //factory_display.drawString(0, 10, "  Luz roja");
    factory_display.display();
    delay(1500);
    digitalWrite(RedLED, HIGH);
    Serial.println(String(rxpacket));

    colorWipe(strip.Color(255, 0, 0), 0);
    delay(500);// Green
    colorWipe(strip.Color(0, 0, 0), 0);
    delay(500);// Blue
    //colorWipe(strip.Color(0, 0, 0), 0);
    colorWipe(strip.Color(255, 0, 0), 0);
    delay(500);// Green
    colorWipe(strip.Color(0, 0, 0), 0);
    delay(500);// Blue
    //colorWipe(strip.Color(0, 0, 0), 0);
    colorWipe(strip.Color(255, 0, 0), 0);
    delay(500);// Green
    colorWipe(strip.Color(0, 0, 0), 0);
    delay(50);// Blue
    

    delay(5000);  
  }
  if (String(rxpacket) == "GreenLED") {
    digitalWrite(GreenLED, LOW);
    factory_display.clear();
    factory_display.display();
    factory_display.setFont(ArialMT_Plain_24);
    factory_display.setTextAlignment(TEXT_ALIGN_LEFT);
    //factory_display.drawString(0, 10, "     Verde");
    factory_display.drawString(0, 10, "Green LED");
    factory_display.display();
    delay(2000);
    digitalWrite(GreenLED, HIGH);
    Serial.println(String(rxpacket));
    
    player.playSpecified(2);
    //delay(5000);
    
    colorWipe(strip.Color(0, 255, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 0), 10);
    delay(5);// Blue
    //colorWipe(strip.Color(0, 0, 0), 10);
    colorWipe(strip.Color(0, 255, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 0), 10);
    delay(5);// Blue
    //colorWipe(strip.Color(0, 0, 0), 10);
    colorWipe(strip.Color(0, 255, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 0), 10);
    delay(5);// Blue
    

    delay(5000);  
  }
  if (String(rxpacket) == "YellowLED") {
    digitalWrite(YellowLED, LOW);
    factory_display.clear();
    factory_display.display();
    factory_display.setFont(ArialMT_Plain_24);
    factory_display.setTextAlignment(TEXT_ALIGN_LEFT);
    //factory_display.drawString(0, 10, "     Verde");
    factory_display.drawString(0, 10, "Yellow LED");
    factory_display.display();
    delay(2000);
    digitalWrite(YellowLED, HIGH);
    Serial.println(String(rxpacket));

    player.playSpecified(4);
    //delay(5000);
    
    colorWipe(strip.Color(255, 255, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 0), 10);
    delay(5);// Blue
    //colorWipe(strip.Color(0, 0, 0), 10);
    colorWipe(strip.Color(255, 255, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 0), 10);
    delay(5);// Blue
    //colorWipe(strip.Color(0, 0, 0), 10);
    colorWipe(strip.Color(255, 255, 0), 10);
    delay(5);// Green
    colorWipe(strip.Color(0, 0, 0), 10);
    delay(5);// Blue
    

    delay(5000);  
  }

  {
    //digitalWrite(BlueLED, LOW);//trash
    //Serial.println(String(rxpacket));//trash
    //delay(0);
    //factory_display.clear();
    //factory_display.display();
    //factory_display.setFont(ArialMT_Plain_24);
    //factory_display.setTextAlignment(TEXT_ALIGN_LEFT);
    //factory_display.drawString(0, 10, "     adiós");
    //factory_display.display();
    //delay(1000);
    factory_display.clear();
    factory_display.display();
    delay(1000);
    digitalWrite(BlueLED, HIGH);
    digitalWrite(GreenLED, HIGH);
  }
}
void colorWipe(uint32_t color, int wait) {
  for(int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}  
