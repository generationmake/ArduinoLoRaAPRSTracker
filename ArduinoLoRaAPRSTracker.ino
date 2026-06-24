/* This example shows how to use LoRA APRS on 70 cm with the Arduino MKR WAN 1300 or 1310
 *
 */

/**************************************************************************************
 * INCLUDE
 **************************************************************************************/

#include <SPI.h>
//#include <LoRa.h>
//#include <MKRWAN.h>
#include "SX126x.h"
#include <ArduinoNmeaParser.h>

#define RF_FREQUENCY         433775000
#define TX_OUTPUT_POWER      22
#define LORA_BANDWIDTH       4
#define LORA_SPREADING_FACTOR 9
#define LORA_CODINGRATE      4
#define LORA_PREAMBLE_LENGTH 8

#define MOSI      19
#define MISO      16
#define SCK       17
#define LORA_NSS  10
#define LORA_RST  9
#define LORA_BUSY 2
#define LORA_DIO1 3

SX126x LoRa(LORA_NSS, LORA_RST, LORA_BUSY, LORA_DIO1, SCK, MISO, MOSI);

/**************************************************************************************
 * CONSTANTS
 **************************************************************************************/

static char const CALLSIGN[]="xxxxxx";
static char const SYMBOLCODE='>';     // [ = Jogger; b = Bicycle; < = Motorcycle; > = Car; k = Truck
static char const SSID[]="4";     // 4 = Bicycle; 10 = Motorcycle; 9 = Car; 14 = Truck

/**************************************************************************************
 * FUNCTION DECLARATION
 **************************************************************************************/

void onGgaUpdate(nmea::GgaData const);
void sendposition(float, float);
const char *createaprscoords(float, float);

/**************************************************************************************
 * GLOBAL VARIABLES
 **************************************************************************************/

ArduinoNmeaParser parser(NULL, onGgaUpdate);
int counter = 0;

/**************************************************************************************
 * SETUP/LOOP
 **************************************************************************************/

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on

#ifdef ARDUINO_ARCH_RP2040
  set_sys_clock_khz(48000, false);  // set pico clk to 48 MHz only on RP2040
#endif

  Serial.begin(9600);
  Serial1.begin(9600);
//  while (!Serial);

  Serial.println("LoRa APRS Sender");

// Configure LoRa module to transmit and receive at the LoRa APRS frequency
//  if (!LoRa.begin(433775000)) {
//    Serial.println("Starting LoRa failed!");
//    while (1);
//  }
//  LoRa.setSpreadingFactor(12);
//  LoRa.setTxPower(20);
//  LoRa.enableCrc();
//  LoRa.sleep(); // send LoRa Module to sleep mode

  int err = LoRa.begin(
      SX126X_PACKET_TYPE_LORA,   // LoRa mode
      RF_FREQUENCY,              // frequency in Hz
      TX_OUTPUT_POWER,           // tx power in dBm
      SX126X_DIO3_OUTPUT_1_8     // TCXO VDD voltage via Dio3
  );
  if (err != 0) {
    Serial.print("LoRa.begin error: ");
    Serial.println(err);
    while (1);
  }
  LoRa.LoRaConfig(
      LORA_SPREADING_FACTOR,
      LORA_BANDWIDTH,
      LORA_CODINGRATE,
      LORA_PREAMBLE_LENGTH,
      true,   // CRC on
      false   // invertIQ
  );

  digitalWrite(LED_BUILTIN, LOW);   // turn the LED off
}

void loop()
{
  while (Serial1.available()) {
    parser.encode((char)Serial1.read());
  }
}

/**************************************************************************************
 * FUNCTION DEFINITION
 **************************************************************************************/
void onGgaUpdate(nmea::GgaData const gga)
{
  if (gga.fix_quality != nmea::FixQuality::Invalid)
  {
    sendposition(gga.latitude,gga.longitude,gga.altitude);
  }
}

const char *maidenhead(float lat, float lon)
{
  static char locator[7]="000000";  // create buffer
  int x, y;
  locator[0]=(int)(lon+180.0)/20+65;
  locator[1]=(int)(lat+90.0)/10+65;
  locator[2]=((int)(lon+180.0)%20)/2+48;
  locator[3]=(int)(lat+90.0)%10+48;
  locator[4]=(int)(((lon/2+90.0)-(int)(lon/2+90.0))*24.0)+65;
  locator[5]=(int)(((lat+90.0)-(int)(lat+90.0))*24.0)+65;
  return locator;
}

const char *createaprscoords(float lat, float lon) {
  static char buf[19]="0000.00X/00000.00X";

  if(lat<0) buf[7]='S';
  else buf[7]='N';
  buf[0]=(int)(lat)/10+48;
  buf[1]=(int)(lat)%10+48;
  float latfract=lat-(int)lat;
  latfract=latfract*60.0;
  buf[2]=(int)(latfract)/10+48;
  buf[3]=(int)(latfract)%10+48;
  buf[5]=(int)(latfract*10)%10+48;
  buf[6]=(int)(latfract*100)%10+48;

  if(lon<0) buf[17]='W';
  else buf[17]='E';
  buf[9]=(int)(lon/100)%10+48;
  buf[10]=(int)(lon/10)%10+48;
  buf[11]=(int)(lon)%10+48;
  float lonfract=lon-(int)lon;
  lonfract=lonfract*60.0;
  buf[12]=(int)(lonfract)/10+48;
  buf[13]=(int)(lonfract)%10+48;
  buf[15]=(int)(lonfract*10)%10+48;
  buf[16]=(int)(lonfract*100)%10+48;

  return buf;
}
const char *createcompressedaprscoords(float lat, float lon, float alt, char symbolcode) {
  static char buf[19]="/YYYYXXXX$ sT";

  int ilat=380926*(90-lat);
  buf[1]=ilat/(91*91*91)+33;
  buf[2]=(ilat%(91*91*91))/(91*91)+33;
  buf[3]=(ilat%(91*91))/(91)+33;
  buf[4]=ilat%(91)+33;
  int ilon=190463*(180+lon);
  buf[5]=ilon/(91*91*91)+33;
  buf[6]=(ilon%(91*91*91))/(91*91)+33;
  buf[7]=(ilon%(91*91))/(91)+33;
  buf[8]=ilon%(91)+33;

  buf[9]=symbolcode;

  if(alt!=NULL)
  {
    float alt_feet=alt*3.281;

    int ialt_cs=log(alt_feet)/log(1.002);
    buf[10]=(ialt_cs%(91*91))/(91)+33;
    buf[11]=ialt_cs%(91)+33;

    buf[12]=81;
  }

  return buf;
}
const char *createaprsalt(float alt) {
  static char buf[10]="/A=000000";

  float alt_feet=alt*3.281;
  buf[3]=(int)(alt_feet/100000)%10+48;
  buf[4]=(int)(alt_feet/10000)%10+48;
  buf[5]=(int)(alt_feet/1000)%10+48;
  buf[6]=(int)(alt_feet/100)%10+48;
  buf[7]=(int)(alt_feet/10)%10+48;
  buf[8]=(int)(alt_feet)%10+48;

  return buf;
}

void sendposition(float lat, float lon, float alt) {
  static unsigned long prev_tx = 0;
  unsigned long const now = millis();
  /* tx data every 2 minutes = 120000 ms */
//  Serial.println();
//  Serial.print(createcompressedaprscoords(lat, lon, alt, SYMBOLCODE));
  if((now - prev_tx) > 120000)
  {
    prev_tx=now;
    static int count = 0;
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    Serial.println("");
    Serial.println("Sending APRS packet: ");

    char lorabuf[100]="";
// send packet
//    LoRa.beginPacket();
  // LoRa APRS Header:
    lorabuf[0]='<';
    lorabuf[1]=0xFF;
    lorabuf[2]=0x01;
    lorabuf[3]=0;
  // APRS Data:
    strcat(lorabuf, CALLSIGN); // callsign
    strcat(lorabuf, "-");  // SSID delimiter
    strcat(lorabuf, SSID);  // SSID specified icon
    strcat(lorabuf, ">APZ666,WIDE1-1:!");  // software version APZ666 = experimental, APRS data type identifier = ! = Position without timestamp
// clear text
//    LoRa.print(createaprscoords(lat, lon)); // from gps data, using primary symbol table
//    LoRa.write(SYMBOLCODE); // symbol code
//    if(alt!=NULL) LoRa.print(createaprsalt(alt)); // altitude only when available

//compressed
    strcat(lorabuf, createcompressedaprscoords(lat, lon, alt, SYMBOLCODE)); // from gps data, using primary symbol table

    if(count==0) strcat(lorabuf, "LoRa Arduino MKR WAN 1300"); // send comment every 10 messages

#ifdef ARDUINO_ARCH_RP2040
    if(count==5)
    {
      strcat(lorabuf, " B=");
      float bat_voltage=analogRead(A0)/1024*3.0*3.0;
//      LoRa.print(bat_voltage);
      strcat(lorabuf, " T=");
//      LoRa.print(analogReadTemp(3.0));
    }
#endif

    LoRa.Send((uint8_t *)lorabuf, sizeof(lorabuf),false);

    LoRa.Sleep(true); // send LoRa Module to sleep mode

    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    count++;  // counter for comment
    if(count>10) count = 0;
  }
}
