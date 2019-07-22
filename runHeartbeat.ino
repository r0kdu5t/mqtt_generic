/**
   The heartbeat takes care of both patting the watchdog and reporting sensor values
*/
void runHeartbeat()
{
  if ((millis() - watchdogLastResetTime) > WATCHDOG_RESET_INTERVAL) // Is it time to run yet?
  {
    dallas.requestTemperatures(); // Send the command to get temperatures
    //humidity = dht.readHumidity();
    //temperature = dht.readTemperature();

    //    char tempC[10];
    //    dtostrf(temperature,1,2,tempC);
    //    char relH[10];
    //    dtostrf(humidity,1,2,relH);
    char* temp;

    char message_buffer[100];
    temp = dtostrf(dallas.getTempCByIndex(0), 5, 2, message_buffer);

#ifdef DEBUG_PRINT
    Serial.println(temp);
#endif

    //client.publish(temperatureTopic, tempC);
//    if (client.publish(topic, temp))
//    {
      patWatchdog();  // Only pat the watchdog if we successfully published to MQTT
#ifdef DEBUG_PRINT
    Serial.println("Pat the Dog!");
#endif      
//    }
    // The interval timer is updated inside patWatchdog()
  }
}

//void getTemp()
//{
//
//  dallas.requestTemperatures(); // Send the command to get temperatures
//
//  char* temp;
//  unsigned long tempTimeout = 0;
//  char message_buffer[100];
//  temp = dtostrf(dallas.getTempCByIndex(0), 5, 2, message_buffer);
//
//  // push each stright out via mqtt
//  if ( (millis() - tempTimeout) > 10000 ) {
//    // if (millis() > (time + 150000)) {
//    tempTimeout = millis();
//    client.publish(topic, temp);
//    delay( 1000 );
//
//#ifdef DEBUG_PRINT
//    Serial.println(temp);
//
//#endif
//  }
//
//} // end getTemp()
