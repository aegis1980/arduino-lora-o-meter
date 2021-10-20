// gps stuff & compass
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>

// lcd stuff
#include <LiquidCrystal.h>

// lora stuff
#include <SPI.h>
#include <RH_RF95.h>

//button stuff


// gps stuff
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// Assign a Unique ID to the HMC5883 Compass Sensor
Adafruit_HMC5883_Unified mag = Adafruit_HMC5883_Unified(12345);

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

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

// Draguino vrsion of Radiohead implements rf95.setSyncWord()
void setSyncWord(int sw){
  rf95.spiWrite(RH_RF95_REG_39_SYNC_WORD,sw);
}

// lora-a-meter stuff
float lat,lng,rssi;

void displaySensorDetails(void)
{
  sensor_t sensor;
  mag.getSensor(&sensor);
  Serial.println("------------------------------------");
  Serial.print  ("Sensor:       "); Serial.println(sensor.name);
  Serial.print  ("Driver Ver:   "); Serial.println(sensor.version);
  Serial.print  ("Unique ID:    "); Serial.println(sensor.sensor_id);
  Serial.print  ("Max Value:    "); Serial.print(sensor.max_value); Serial.println(" uT");
  Serial.print  ("Min Value:    "); Serial.print(sensor.min_value); Serial.println(" uT");
  Serial.print  ("Resolution:   "); Serial.print(sensor.resolution); Serial.println(" uT");  
  Serial.println("------------------------------------");
  Serial.println("");
  delay(500);
}


void displayGpsInfo()
{
  // Prints the location if lat-lng information was recieved
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
  }
  // prints invalid if no information was recieved in regards to location.
  else
  {
    Serial.print(F("INVALID"));
  }

  Serial.print(F("  Date/Time: "));
  // prints the recieved GPS module date if it was decoded in a valid response.
  if (gps.date.isValid())
  {
    Serial.print(gps.date.month());
    Serial.print(F("/"));
    Serial.print(gps.date.day());
    Serial.print(F("/"));
    Serial.print(gps.date.year());
  }
  else
  {
    // prints invalid otherwise.
    Serial.print(F("INVALID"));
  }

  Serial.print(F(" "));
  // prints the recieved GPS module time if it was decoded in a valid response.
  if (gps.time.isValid())
  {
    if (gps.time.hour() < 10) Serial.print(F("0"));
    Serial.print(gps.time.hour());
    Serial.print(F(":"));
    if (gps.time.minute() < 10) Serial.print(F("0"));
    Serial.print(gps.time.minute());
    Serial.print(F(":"));
    if (gps.time.second() < 10) Serial.print(F("0"));
    Serial.print(gps.time.second());
    Serial.print(F("."));
    if (gps.time.centisecond() < 10) Serial.print(F("0"));
    Serial.print(gps.time.centisecond());
  }
  else
  {
    // Print invalid otherwise.
    Serial.print(F("INVALID"));
  }
  Serial.println();
  if(mag.begin())
  {
    //displayCompassInfo();
  }
}

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
  setSyncWord(0x34);
  
}

void loop() {
   // This sketch displays information every time a new sentence is correctly encoded from the GPS Module.
  while (ss.available() > 0)
    if (gps.encode(ss.read()))
      displayGpsInfo();
}