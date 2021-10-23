// gps stuff & compass
#include <TinyGPS++.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_HMC5883_U.h>
#include <math.h>


// lcd stuff
#include <LiquidCrystal.h>

// lora stuff
#include <SPI.h>
#include <RH_RF95.h>

//button stuff
#include "avdweb_Switch.h"


// gps stuff
static const int RXPin = 30 , TXPin = 32;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// lcd stuff
const int rs = 31, en = 33, d4 = 35, d5 = 37, d6 = 39, d7 = 41;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// lora stuff
// Singleton instance of the radio driver
RH_RF95 rf95;
float frequency = 915.0;  //frequency settings
int txPower = 13;
const char *node_id = "<1531236>";  //From LG01 via web Local Channel settings on MQTT.Please refer <> dataformat in here.
uint8_t datasend[70];
unsigned int count = 1;


//switch & dial stuff
Switch lcdBtn(MODE_BTN_PIN);
const int potPin = 2;    // select the input pin for the potentiometer

/// Draguino vrsion of Radiohead implements rf95.setSyncWord()
/// https://github.com/dragino/RadioHead/commit/43720d1ed6fbf6719968f5f8cfad21a561b9bbe2
void setSyncWord(int sw){
  rf95.spiWrite(RH_RF95_REG_39_SYNC_WORD,sw);
}

// lora-a-meter stuff
float lat,lng,alt;
int rssi;
int snr;
char lat_1[16]={"\0"},lng_1[16]={"\0"},rssi_1[8]={"/0"}, snr_1[8]={"/0"};


void sendLoRaMessage()
{

    char data[50] = "\0";
    for(int i = 0; i < 50; i++)
    {
       data[i] = node_id[i];
    }

    dtostrf(lat,0,1,lat_1);
    dtostrf(lng,0,1,lng_1);

  // Serial.println(tem_1);
    strcat(data,"lat=");
    strcat(data,lat_1);
    strcat(data,"lng=");
    strcat(data,lng_1);
    strcat(data,"rssi=");
    strcat(data,rssi_1);
    strcat(data,"snr=");
    strcat(data,snr_1);
    strcpy((char *)datasend,data);

    Serial.println(F("Sending data to LG01"));
  
   
    rf95.send(datasend,sizeof(datasend));  
    Serial.println(F("Data sent to LG01"));
    rf95.waitPacketSent();  // Now wait for a reply
  
    uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
    uint8_t len = sizeof(buf);

     if (rf95.waitAvailableTimeout(3000))
  { 
    // Should be a reply message for us now   
    if (rf95.recv(buf, &len))
   {
     
      Serial.print("got reply from LG01: ");
      Serial.println((char*)buf);
      Serial.print("RSSI: ");
      Serial.println(rf95.lastRssi(), DEC);
      
      rf95.lastSNR();

    }
    else
    {
      Serial.println("recv failed");
    }
  }
  else
  {
    Serial.println("No reply, is LoRa server running?");
  }
  delay(5000);
}






bool displayGpsInfo()
{
  // Prints the location if lat-lng information was recieved
  Serial.print(F("Location: ")); 
  if (gps.location.isValid())
  {
    Serial.print(gps.location.lat(), 6);
    Serial.print(F(","));
    Serial.print(gps.location.lng(), 6);
    lcd.setCursor(0,0);
    lcd.print(gps.location.lat(),3);
    lcd.print(",");
    lcd.print(gps.location.lng(),3);

    lcd.setCursor(0,1);
    if (gps.speed.isValid()){
      lcd.print(gps.speed.kmph(),1);
      lcd.print("kph ");
    }
    lcd.print(gps.speed.kmph(),1);
    lcd.print("kph ");
    if (gps.altitude.isValid()){
       lcd.print(gps.altitude.meters(),1);
       lcd.print("m");
    }
   
  }
  // prints invalid if no information was recieved in regards to location.
  else
  {
    Serial.print(F("INVALID"));
    lcd.clear();
    lcd.print("No GPS signal");
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
}

void setup() {
  Serial.begin(9600);
  Serial1.begin(GPSBaud);

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.println("Starting up");

  if (!rf95.init()){
    lcd.clear();
    lcd.println("rf95 init failed");
  } 

  
  rf95.setFrequency(frequency);
  rf95.setTxPower(txPower);
  setSyncWord(0x34);

  
}

void checkDisplayMode(){
  int val = analogRead(potPin);
  if 
}

void loop() {

  checkDisplayMode();
   // This sketch displays information every time a new sentence is correctly encoded from the GPS Module.
  while (Serial1.available() > 0)
    if (gps.encode(Serial1.read()))
      if (displayGpsInfo()){
        sendLoRaMessage();
        displayGpsInfo();
      };
}