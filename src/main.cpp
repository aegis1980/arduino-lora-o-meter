// gps stuff
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

// lcd stuff
#include <LiquidCrystal.h>

// lora stuff
#include <SPI.h>
#include <RH_RF95.h>

// gps stuff
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;


// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

TinyGPSCustom totalGPGSVMessages(gps, "GPGSV", 1); // $GPGSV sentence, first element
TinyGPSCustom messageNumber(gps, "GPGSV", 2);      // $GPGSV sentence, second element
TinyGPSCustom satNumber[4]; // to be initialized later
TinyGPSCustom elevation[4];
bool anyChanges = false;
unsigned long linecount = 0;

// lcd stuff
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// lora stuff
// Singleton instance of the radio driver
RH_RF95 rf95;
float frequency = 915.0;  //frequency settings
char *node_id = "<1531236>";  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here.
uint8_t datasend[36];
unsigned int count = 1;

// lora-a-meter stuff
float lat,lng,rssi;

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("Starting up...GPS");

  Serial.println(F("Simple Test with TinyGPS++ and attached NEO-6M GPS module"));
  Serial.print(F("Testing TinyGPS++ library v. ")); Serial.println(TinyGPSPlus::libraryVersion());
  Serial.println();
  displaySensorDetails();


  if (!rf95.init()) Serial.println(F("rf 95 init failed"));
  
  rf95.setFrequency(frequency);
  rf95.setTxPower(13);
  rf95.setSyncWord(0x34);
}

void loop() {
  // put your main code here, to run repeatedly:
}