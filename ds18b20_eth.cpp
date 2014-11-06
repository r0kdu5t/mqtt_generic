#include <SPI.h>
#include <Ethernet.h>
#include <OneWire.h>

// Attempt to automatically configure everything. Set a default MAC
// address to be replaced later, and get the IP address from DHCP
byte mac[] = {  
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };  // Try to replace with DS MAC

// Initialise a DS18B20 temperature sensor
OneWire ds(2); // Set up DS18B20 sensor on pin D2

/**
 * Setup
 */
void setup()
{
  byte i;
  byte dsAddress[8];
  
  Serial.begin( 38400 );
  delay( 500 ); // Give the Ethernet chipset time to fully initialise

  Serial.println( "Searching for DS18B20..." );
  ds.reset_search();  // Start the search with the first device
  if( !ds.search(dsAddress) )
  {
    Serial.println( "none found. Using default MAC address." );
  } else {
    Serial.println( "success. Setting MAC address:" );
    Serial.print( " DS18B20 ROM  =" );
    for( i = 0; i < 8; i++)
    {
      Serial.write(' ');
      Serial.print( dsAddress[i], HEX );
    }
    Serial.println();
    
    // Offset array to skip DS18B20 family code, and skip mac[0]
    mac[1] = dsAddress[3];
    mac[2] = dsAddress[4];
    mac[3] = dsAddress[5];
    mac[4] = dsAddress[6];
    mac[5] = dsAddress[7];
  }
  
  Serial.print( " Ethernet MAC =" );
  for( i = 0; i < 6; i++ )
  {
    Serial.write( ' ' );
    Serial.print( mac[i], HEX );
  }
  Serial.println();

  // start the Ethernet connection:
  Serial.print( "Eth: " );
  Ethernet.begin( mac );
  Serial.print( "My IP address: " );
  Serial.println( Ethernet.localIP() );
}

void loop()
{

}
