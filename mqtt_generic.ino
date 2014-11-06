/**************************************************** 
 * mqtt_generic
 * Andrew Sands - r0kdu5t@theatrix.org.nz
 *
 * Forked from mqtt_temperature by Jon Archer
 * 
 * Sketch to take the temperature from an attached 
 * Dallas sensor and post it over MQTT. 
 * Can be configured to use the DS18B20 to generate
 * a MAC address for ethernet use from 
 * the Dallas sensors unique ID or can retrieve the
 * MAC address from a Microchip 24AA125E48 I2C ROM.
 *
 * This is a starting point sketch.
 * 
 ****************************************************/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "config.h"
#ifdef MAC_MCP
#include <Wire.h>
#define I2C_ADDRESS 0x50
#endif

byte mac[]= { 0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02 };

OneWire node_id(ONE_WIRE_BUS);
DallasTemperature nodeaddr(&node_id);


EthernetClient ethClient;  // Ethernet object
PubSubClient client( MQTT_SERVER, 1883, callbackMQTT, ethClient); // MQTT object
//DallasTemperature dallas(&ds);

void ethernetFromDS(){
    byte i;
    byte dsAddress[8];
    delay( 500 );
#ifdef DEBUG_PRINT
    Serial.print ("Searching for DS18B20...");
#endif
    node_id.reset_search();

  if ( !node_id.search(dsAddress) )
  {
#ifdef DEBUG_PRINT
    Serial.println("none found. Using specified MAC Address.");
#endif
  } else {
#ifdef DEBUG_PRINT
    Serial.print( "Success! \nSetting MAC address...." );
#endif
  mac[1] = dsAddress[3];
  mac[2] = dsAddress[4];
  mac[3] = dsAddress[5];
  mac[4] = dsAddress[6];
  mac[5] = dsAddress[7];
  }
#ifdef DEBUG_PRINT  
  Serial.print( "Ethernet MAC =" );
  for( i = 0; i < 6; i++ )
  {
    Serial.write( ' ' );
    Serial.print( mac[i], HEX );
  }
  Serial.println();
#endif
/*
  if (Ethernet.begin(mac) == 0) {
#ifdef DEBUG_PRINT
    Serial.println("Failed to configure Ethernet using DHCP");
#endif
    for(;;)
      ;
  } */
//wait for IP address
  while (Ethernet.begin(mac) != 1) {
    Serial.println("Error getting IP address via DHCP, trying again...");
    // blink an LED to identify error!
    delay(5000);
  }  

#ifdef DEBUG_PRINT
  Serial.print("IP address: ");
    for (byte thisByte = 0; thisByte < 4; thisByte++) {
    // print the value of each byte of the IP address:
      Serial.println(Ethernet.localIP()[thisByte], DEC);
    }
#endif  
}

//Start MQTT goodness
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
}

void checkMQTT() {
    if(!client.connected()) {
    if (client.connect(CLIENT_ID)) {
#ifdef DEBUG_PRINT
      Serial.println(F("MQTT Reconecting"));
#endif
    } 
  } 
} // end checkMQTT()


void getTemp()
{

   nodeaddr.requestTemperatures(); // Send the command to get temperatures
    
      char* temp;
      unsigned long tempTimeout = 0;
      char message_buffer[100];
      temp = dtostrf(nodeaddr.getTempCByIndex(0), 5, 2, message_buffer);
      
      // push each stright out via mqtt
       if ( (millis() - tempTimeout) > 10000 ) {
       // if (millis() > (time + 150000)) {
    tempTimeout = millis();
    client.publish(TEMP_TOPIC, temp);
    delay( 1000 );
    
#ifdef DEBUG_PRINT
      Serial.println(temp);

#endif
        }
  
} // end getTemp()



void setup() {
  Serial.begin(9600);

  //Start Ethernet using mac formed from DS
  #ifdef MAC_DS
    #ifdef DEBUG_PRINT
      Serial.println("Starting ethernetFromDS...");
    #endif
    ethernetFromDS();
  #endif
/*
  //Start Ethernet using mac formed from DS
  #ifdef MAC_MCP
    #ifdef DEBUG_PRINT
      Serial.println("Starting ethernetMCP...");
    #endif
    ethernetFromDS();
  #endif
 */ 
//Start the dallas sensor
nodeaddr.begin();

// Start MQTT
checkMQTT();

// short delay to make sure we're happy
delay(100);
}


void loop()
{
   client.loop();
//Get the temperatures 
  getTemp();
  
// are we still connected to MQTT
  checkMQTT();
}

/*

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

void ethernetMCP() {
  //
  // * Retrieve the MAC address from a Microchip 24AA125E48 I2C ROM, and report it
  // * to the serial console at 57600bps. The I2C address of the ROM is set to 0x50,
  // * which assumes both the address pins are tied to 0V.
  //
  char tmpBuf[17];
  // Join i2c bus (I2C address is optional for the master)
  Wire.begin(); // May need to move to setup
  
  // Could relpace with a simple delay?
  for(int i = 0; i < 30; i++)
  {
    Serial.println(" ");
  }
  #ifdef DEBUG_PRINT
    Serial.println("Starting test for MAC address ROM");
    Serial.print("Getting MAC: ");
  #endif
  
  mac[0] = readRegister(0xFA);
  mac[1] = readRegister(0xFB);
  mac[2] = readRegister(0xFC);
  mac[3] = readRegister(0xFD);
  mac[4] = readRegister(0xFE);
  mac[5] = readRegister(0xFF);
  
  #ifdef DEBUG_PRINT
    sprintf(tmpBuf, "%02X:%02X:%02X:%02X:%02X:%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    Serial.println(tmpBuf);
    Serial.println(" TEST OK");
  #endif

}

 */
