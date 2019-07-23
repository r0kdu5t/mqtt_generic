/****************************************************
   mqtt_generic

   Based heavily upon Arduino_mqtt_temp by Jon Archer

   Also influenced by SuperHouse Automation Pty Ltd <info@superhouse.tv>

   This is essentially a starting point for MQTT based devices.

 ****************************************************/


#include <SPI.h>                    // For networking
#include <Ethernet.h>               // For networking
#include <PubSubClient.h>           // For MQTT
#include <OneWire.h>                // 1-Wire bus
#include <DallasTemperature.h>      // 1-Wire sensors like DS18B20
#include "CONFIG.h"                 // configuration file

/*--------------------------- Configuration ------------------------------*/
/* Network config */
/* MQTT Settings */
byte broker[] = MQTT_BROKER;
//const char topic[] = TEMP_TOPIC;
//const char topic_base[] = TOPIC_BASE;
#define TOPIC_BASE "tele/HitGirl/"                   // Format of '%PREFIX% / %NAME% /' Remember to change %NAME% for each different device!
//const char client_id[] = CLIENT_ID;
static uint8_t mac[] = MAC;
// May remove the following soon!
const char* commandTopic = "cmnd/";               // MQTT topic to subscribe for commands - REMOVE from reconnect!!
const char* statusTopic  = "stat/";               // MQTT topic to publish status reports
const byte iWire = ONE_WIRE_BUS;
char client_id[8];

/* Watchdog Timer Settings */
/*
   Always TRUE so this is not needed.
   #define ENABLE_EXTERNAL_WATCHDOG        true       // true / false
*/
#define WATCHDOG_PIN                    6          // Output to pat the watchdog
#define WATCHDOG_PULSE_LENGTH           50         // Milliseconds
#define WATCHDOG_RESET_INTERVAL         30000      // Milliseconds. Also the period for sensor reports.
long watchdogLastResetTime = 0;

/* Temperature Sensor Settings */
OneWire ds(iWire);

//Start MQTT goodness
void callbackMQTT(char* topic, byte* payload, unsigned int length) {
}


/* Ok HAL, build me some objects */
EthernetClient ethClient;             // Ethernet
PubSubClient   client(ethClient);     // MQTT
DallasTemperature dallas(&ds);        // Temperature sensor


void setup()
{
  Serial.begin(9600);

  pinMode(WATCHDOG_PIN, OUTPUT);
  digitalWrite(WATCHDOG_PIN, LOW);

  //Start Ethernet using mac formed from DS
  ethernetFromDS();

  //Start the dallas sensor
  dallas.begin();

  // Testing substring extraction.
  //char client_id[8];
  //strncpy(client_id, &topic_base[5], 7);
  strncpy(client_id, &TOPIC_BASE[5], 7);
  client_id[7] = '\0';
  //Serial.println(topic_base);
  Serial.println(TOPIC_BASE);
  Serial.println(client_id);
  
  // Set up MQTT
  client.setServer(broker, 1883);
  client.setCallback(callbackMQTT);

  // short delay to make sure we're happy
  //delay(100);
}


void loop() {
  // Make sure we're connected to the MQTT broker
  if (!client.connected()) {
#ifdef DEBUG_PRINT
    Serial.println(F("MQTT Reconecting"));
#endif
    reconnect();
  }
  client.loop();

  runHeartbeat();

}
