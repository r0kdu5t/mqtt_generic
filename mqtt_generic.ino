/*************************************************************************** 
 * mqtt_generic
 * Andrew Sands - r0kdu5t@theatrix.org.nz
 *
 * Forked from mqtt_temperature by Jon Archer
 *
 * The arduino can be configured to use either;
 * a DS18B20 to generate a MAC address for ethernet use from the Dallas sensors unique ID
 * or can retrieve the MAC address from a Microchip 24AA125E48 I2C ROM.
 *
 * As configured this code will create a mqtt node that will emit a beacon.
 * 
 ***************************************************************************/
//#define MAC_DS  	// Use DS for MAC
//#define DS_TEMP       // Use DS for temperature
#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>

#ifdef MAC_DS // TODO - add OR function with DS_TEMP!
#include <OneWire.h>
#endif // MAC_DS
#ifdef DS_TEMP
#include <DallasTemperature.h>
#endif // DS_TEMP
#include "config.h"
#ifdef MAC_MCP
#include <Wire.h>
#define I2C_ADDRESS 0x50
#endif
#ifdef HUMID
#include "DHT.h"
#endif // HUMID

byte mac[]= { 
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };
char macstr[18];

#ifdef MAC_DS || def DS_TEMP
OneWire node_id(ONE_WIRE_BUS);
#endif // MAC_DS || def DS_TEMP
#ifdef DS_TEMP 
DallasTemperature nodeaddr(&node_id);
#endif // DS_TEMP
#ifdef HUMID
DHT dht(DHTPIN, DHTTYPE);
#endif // HUMID

EthernetClient ethClient;  // Ethernet object
PubSubClient client( MQTT_SERVER, 1883, callbackMQTT, ethClient); // MQTT object

//#ifdef BEACON || def HUMID
long previousMillis = 0;
#ifdef DEBUG_PRINT
#define INTERVAL 10000
#else#
#define INTERVAL 60000
#endif
//#endif // BEACON

#ifdef BEACON
/**
 * activateBeacon
 */
void activateBeacon() {
  // Beacon code in this routine.
  char topic[40];
  memset(topic, '\0', 40);
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis > long(INTERVAL)) {
    // save the last time
    previousMillis = currentMillis;
    
    snprintf(topic, 40, "raw/%s/beacon", macstr);
    client.publish(topic, BEACON_TEXT);
    
#ifdef DEBUG_PRINT
    Serial.print(topic);
    Serial.print(" : ");
    Serial.println(BEACON_TEXT);
#endif // DEBUG_PRINT
  } 
}
#endif // activateBeacon()

#ifdef MAC_DS
void ethernetFromDS() {
  byte i;
  byte dsAddress[8];
  delay( 500 );
#ifdef DEBUG_PRINT
  Serial.print ("Searching for DS18B20...");
#endif // DEBUG_PRINT
  node_id.reset_search();

  if ( !node_id.search(dsAddress) )
  {
#ifdef DEBUG_PRINT
    Serial.println("none found. Using specified MAC Address.");
#endif // DEBUG_PRINT
  } 
  else {
#ifdef DEBUG_PRINT
    Serial.print( "Success! \nSetting MAC address...." );
#endif // DEBUG_PRINT
    mac[1] = dsAddress[3];
    mac[2] = dsAddress[4];
    mac[3] = dsAddress[5];
    mac[4] = dsAddress[6];
    mac[5] = dsAddress[7];
  }
  // Generate macstr for node naming convention?
  snprintf(macstr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);  
#ifdef DEBUG_PRINT
  Serial.println();
  Serial.print("Ethernet MAC = (");
  Serial.print(macstr);
  Serial.println(")...");  
#endif // DEBUG_PRINT
  //wait for IP address
  while (Ethernet.begin(mac) != 1) {
    Serial.println("Error getting IP address via DHCP, trying again...");
    // TODO: turn on or blink an LED to identify error!
    delay(5000);
  }  

#ifdef DEBUG_PRINT
  Serial.print("IP address: ");
  Serial.println( Ethernet.localIP() );  
#endif // DEBUG_PRINT
}
#endif // MAC_DS

//Start MQTT goodness
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
}

boolean connect_mqtt() {
    // Use macstr for node naming convention?
    // Format == "%02x:%02x:%02x:%02x:%02x:%02x"
    Serial.print("MQTT...");
    if (client.connect(macstr)) {
        Serial.println("connected");
        char topic[40];
        snprintf(topic, 40, "raw/state/%s", macstr);
        client.publish(topic, "OnLine");
        return true;
        }
    Serial.println("connection failed.");
    return false;
} // end connect_mqtt()

#ifdef DS_TEMP
void getTemp()
{
  //char* temp;
  unsigned long tempTimeout = 0;
  //char message_buffer[100];
  char topic[50];
  memset(topic, '\0', 50);
  
  
  nodeaddr.requestTemperatures(); // Send the command to get temperatures

  //temp = dtostrf(nodeaddr.getTempCByIndex(0), 5, 2, message_buffer);
  float celsius = nodeaddr.getTempCByIndex(0);
  // Index 0 means first device on the 1-Wire bus.

  // push out via mqtt every INTERVAL
  if ( (millis() - tempTimeout) > 10000 ) {
    // if (millis() > (time + 150000)) {
    tempTimeout = millis();
    //client.publish(TEMP_TOPIC, temp);
    char charMsg[10];
    memset(charMsg, '\0', 10);
    dtostrf(celsius, 4, 2, charMsg);
    snprintf(topic, 50, "raw/%s/temperature", macstr);
    client.publish(topic, charMsg);
    delay( 1000 );

#ifdef DEBUG_PRINT
    //Serial.println(temp);
    Serial.println(charMsg);
#endif // DEBUG_PRINT
  }

} // end getTemp()
#endif // DS_TEMP 

#ifdef HUMID
/**
 * getHumid
 */
void getHumid() {
  char topic[50];
  memset(topic, '\0', 50);
  unsigned long currentMillis = millis();
  
  if (currentMillis - previousMillis > long(INTERVAL)) {
    // save the last time
    previousMillis = currentMillis;
    
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // check if returns are valid, if they are NaN (not a number) then something went wrong!
  if (isnan(t) || isnan(h)) {
    #ifdef DEBUG_PRINT
    // TODO: Find where "Read fail" comes from.
    Serial.println("Failed to read from DHT");
    #endif // DEBUG_PRINT
    snprintf(topic, 40, "raw/%s/status", macstr);
    client.publish(topic, "Failed to read from DHT");
  } else {
    char charMsg[10];
    memset(charMsg, '\0', 10);
    dtostrf(t, 4, 2, charMsg);
    snprintf(topic, 50, "raw/%s/temperature", macstr);
    client.publish(topic, charMsg);  // publish Temperature.
   
    memset(charMsg, '\0', 10);
    dtostrf(h, 4, 2, charMsg);
    snprintf(topic, 50, "raw/%s/humidity", macstr);
    client.publish(topic, charMsg);  // publish Humidity in %
    }    
#ifdef DEBUG_PRINT
    Serial.print("DHT: ");
    Serial.print("Temperature: ");
    Serial.print(t);
    Serial.print(" Humidity: ");
    Serial.print(h);
    Serial.println();
#endif // DEBUG_PRINT
  }
}

#endif // HUMID

void setup() {
  Serial.begin(9600);

  //Start Ethernet using mac formed from DS
#ifdef MAC_DS
#ifdef DEBUG_PRINT
  Serial.println("Starting ethernetFromDS...");
#endif // DEBUG_PRINT
  ethernetFromDS();
#endif

  //Start Ethernet using mac formed from DS
#ifdef MAC_MCP
#ifdef DEBUG_PRINT
  Serial.println("Starting ethernetFromMCP...");
#endif // DEBUG_PRINT
  ethernetFromMCP();
#endif

#ifdef DEBUG_PRINT
  Serial.print("DEBUG mode. Beacon INTERVAL set to : ");
  Serial.println(INTERVAL);
#endif // DEBUG_PRINT

#ifdef DS_TEMP
  //Start the dallas sensor
  nodeaddr.begin();
#endif // DS_TEMP

#ifdef HUMID
#ifdef DEBUG_PRINT
  Serial.println("DEBUG mode. HUMID Routine: ");
#endif // DEBUG_PRINT
  dht.begin();
#endif // HUMID

  // short delay to make sure we're happy
  delay(100);
} // end setup()

void loop()
{
#ifdef BEACON
  //Call beacon 
  activateBeacon();
#endif // BEACON  

#ifdef DS_TEMP
  //Get the temperatures 
  getTemp();
#endif // DS_TEMP

#ifdef HUMID
  //Get humidity
  getHumid();
#endif // HUMID
  // are we still connected to MQTT
   if (!client.connected() && !connect_mqtt()) {
       return;
   }
   client.loop();  

   int dhcp_status = Ethernet.maintain();
   /*
   returns:
   0: nothing happened
   1: renew failed
   2: renew success
   3: rebind fail
   4: rebind success
   */
   if (dhcp_status) {
       long now = millis();
       #ifdef DEBUG_PRINT
       Serial.println("DHCP Lease");
       #endif // DEBUG_PRINT
   }  
} // end loop()

#ifdef MAC_MCP
byte readRegister(byte r)
{
  unsigned char v;
  Wire.beginTransmission(I2C_ADDRESS);
  Wire.write(r);  // Register to read
  Wire.endTransmission();

  Wire.requestFrom(I2C_ADDRESS, 1); // Read a byte
  while(!Wire.available())
  {
    // Wait
  }
  v = Wire.read();
  return v;
}
#endif // MAC_MCP

#ifdef MAC_MCP
void ethernetFromMCP() {
  //
  // * Retrieve the MAC address from a Microchip 24AA125E48 I2C ROM.
  // * The I2C address of the ROM is set to 0x50, which assumes 
  // * both the address pins are tied to 0V.
  //
  // Join i2c bus (I2C address is optional for the master)
  Wire.begin(); // May need to move to setup

  // TODO: Relpace with simple delay?
  for(int i = 0; i < 30; i++)
  {
    Serial.print(" ");
  }
  Serial.println(" ");
#ifdef DEBUG_PRINT
  Serial.println("Starting test for MAC address ROM");
  Serial.print("Getting MAC: ");
#endif // DEBUG_PRINT

  mac[0] = readRegister(0xFA);
  mac[1] = readRegister(0xFB);
  mac[2] = readRegister(0xFC);
  mac[3] = readRegister(0xFD);
  mac[4] = readRegister(0xFE);
  mac[5] = readRegister(0xFF);

#ifdef DEBUG_PRINT
  // Generate macstr for node naming convention?
  snprintf(macstr, 18, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  Serial.println(macstr);
  //Serial.println(" TEST OK");
#endif // DEBUG_PRINT
  //wait for IP address
  while (Ethernet.begin(mac) != 1) {
    Serial.println("Error getting IP address via DHCP, trying again...");
    // TODO: turn on or blink an LED to identify error!
    delay(5000);
  }  

#ifdef DEBUG_PRINT
  Serial.print("IP address: ");
  Serial.println( Ethernet.localIP() );  
#endif // DEBUG_PRINT
}
#endif // MAC_MCP

// #ifdef <name_ref> #endif // <name_ref>
// dtostrf(floatVar, minStringWidthIncDecimalPoint, numVarsAfterDecimal, charBuf);
// http://forum.arduino.cc/index.php?topic=103935.0
// http://www.arduino-hacks.com/float-to-string-float-to-character-array-arduino/
