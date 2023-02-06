#include <Wire.h>

#define SDA 21       // Pin for I2C SDA
#define SCL 22      // Pin for I2C SCL          

#include <SHT21_TTGO.h>  // include SHT21 library

TwoWire twi = TwoWire(1);
SHT21_TTGO sht = SHT21_TTGO(&twi);

float temp;   // variable to store temperature
float humidity; // variable to store hemidity

//Libraries for LoRa
#include <SPI.h>
#include <LoRa.h>

//Libraries for OLED Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//packet counter
int counter = 0;

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);

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

const unsigned long UpdateInterval = 0.25 * (60L * 1000000L); // Update delay in microseconds, currently 15-secs (1/4 of minute)

void setup() {
  twi.begin(SDA,SCL);  // begin I2C
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32

  //SPI LoRa pins
  SPI.begin(5, 19, 27, 18);
  
  Serial.println("LoRa Sender starting...");
  LoRa.setPins(18, 14, 26); // LoRa.setPins(CS, RESET, IRQ); 
  if (!LoRa.begin(915E6)) { // Set frequency to 433, 868 or 915MHz
    Serial.println("Could not find a valid LoRa transceiver, check pins used and wiring!");
    delay(1000);
  }
  Serial.println("Sending data packet...");
  Send_and_Display_Sensor_Data();
  start_sleep();  
}

void start_sleep(){
  esp_sleep_enable_timer_wakeup(UpdateInterval);
  //pinMode(BUILTIN_LED,INPUT);     // Set pin-5 to an input as sometimes PIN-5 is used for SPI-SS
  //digitalWrite(BUILTIN_LED,HIGH); // In case it's on, turn LED off, as sometimes PIN-5 on some boards is used for SPI-SS and can be left low
  Serial.println("Starting deep-sleep period... awake for "+String(millis())+"mS");
  delay(100); // 8=Enough time for the serial port to finish at 115,200 baud
  esp_deep_sleep_start();         // Sleep for the prescribed time
}

void loop() {
}

void Send_and_Display_Sensor_Data(){
  LoRa.beginPacket();                                                // Start LoRa transceiver
  LoRa.print(String(sht.getTemperature()));       // Send BME280 temperature reading
  //LoRa.print("Humi: "+String(sht.getHumidity()));           // Send BME280 humidity reading
  LoRa.print(" "+String(sht.getHumidity()));
  LoRa.print(" "+String(sht.getTemperature()));
  //LoRa.print(counter);
  LoRa.endPacket();                                                  // Confirm end of LoRa data packet
  LoRa.sleep();                                                      // Send LoRa transceiver to sleep
  Serial.print("Temperature : "+String(sht.getTemperature())+"°C\n");
  Serial.print("Humidity    : "+String(sht.getHumidity())+"%\n");
  Serial.print(counter);
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.println("Temp: "+String(sht.getTemperature())+" C");
  display.setCursor(0,10);
  display.print("Humi: "+String(sht.getHumidity())+" %");      
  display.display();


  counter++;
  delay(2000);
}
