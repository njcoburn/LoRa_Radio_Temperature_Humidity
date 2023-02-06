#include <SPI.h>
#include <LoRa.h> //https://github.com/sandeepmistry/arduino-LoRa

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

String LoRaData;

// OLED FeatherWing buttons map to different pins depending on board:
#if defined(ESP8266)
  #define BUTTON_A  0
  #define BUTTON_B 16
  #define BUTTON_C  2
#elif defined(ESP32)
  #define BUTTON_A 15
  #define BUTTON_B 32
  #define BUTTON_C 14
#elif defined(ARDUINO_STM32_FEATHER)
  #define BUTTON_A PA15
  #define BUTTON_B PC7
  #define BUTTON_C PC5
#elif defined(TEENSYDUINO)
  #define BUTTON_A  4
  #define BUTTON_B  3
  #define BUTTON_C  8
#elif defined(ARDUINO_FEATHER52832)
  #define BUTTON_A 31
  #define BUTTON_B 30
  #define BUTTON_C 27
#else // 32u4, M0, M4, nrf52840 and 328p
  #define BUTTON_A  9
  #define BUTTON_B  6
  #define BUTTON_C  5
#endif


void setup() {
  Serial.begin(115200);
  Serial.println("LoRa Receiver");
  LoRa.setPins(18, 14, 26);
  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  //initialize OLED
  Serial.println("OLED FeatherWing test");
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  display.clearDisplay();
  display.display();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("LORA RECEIVER ");

  Serial.println("LoRa Receiver Test");

}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket(); 
//  if (packetSize) {
//    // received a packet
//    Serial.println("Received packet '");
//  }
    // read packet
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.println(LoRaData);
      Serial.print((char)LoRa.read());
    }

//     while (LoRa.available()) {
//      LoRaData = LoRa.readString(); // modification
//      Serial.println(LoRaData); 
//    }

    // print RSSI of packet
    //Serial.print(" RSSI was: ");
    //Serial.println(LoRa.packetRssi());

      // Dsiplay information
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("LORA RECEIVER");
    //display.setCursor(0,10);
    //display.print("Received packet:");
    display.setCursor(0,10);
    display.print(LoRaData);
    display.setCursor(0,25);
    display.print("RSSI:");
    display.setCursor(30,25);
    display.print(LoRa.packetRssi());
    display.display();   
 }
