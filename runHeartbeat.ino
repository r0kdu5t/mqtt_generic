/**
   The heartbeat takes care of both patting the watchdog and reporting sensor values
*/
void runHeartbeat()
{
  if ((millis() - watchdogLastResetTime) > WATCHDOG_RESET_INTERVAL) // Is it time to run yet?
  {
    dallas.requestTemperatures(); // Send the command to get temperatures

    float tempValue = dallas.getTempCByIndex(0); // Get value from sensor


#ifdef DEBUG_PRINT
    Serial.println((float)(2.789),1); // This will print: “2.8”
    Serial.println((float)tempValue,2);
#endif

    PublishFloat((char *)"DS18B20", tempValue); // Publish temperature value on topic
////    if (client.publish(topic, temp))
////    {
//      patWatchdog();  // Only pat the watchdog if we successfully published to MQTT
//#ifdef DEBUG_PRINT
//    Serial.println("Pat the Dog!");
//#endif      
////    }
    // The interval timer is updated inside patWatchdog()
  }
}
