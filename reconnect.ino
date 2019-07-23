/**
   Attempt connection to the MQTT broker, and try repeatedly until it succeeds
*/
void reconnect() {
//  // Generate a unique MQTT client ID from our IP address
//  char clientBuffer[50];
//  String clientString = client_id + String(Ethernet.localIP());
//#ifdef DEBUG_PRINT
//    Serial.println(clientString);
//#endif  
//  clientString.toCharArray(clientBuffer, clientString.length() + 1);

  while (!client.connected()) {
#ifdef DEBUG_PRINT
    Serial.print("Attempting MQTT connection...");
#endif
    // Attempt to connect
    //if (client.connect(clientBuffer)) {
    // boolean connect (clientID, willTopic, willQoS, willRetain, willMessage)
    if (client.connect(client_id, (char *)TOPIC_BASE "LWT", 1, 0, "OffLine")) {   
#ifdef DEBUG_PRINT
      Serial.println("connected");
#endif
      // Once connected, publish an announcement...
      Publish((char *)"LWT", (char *)"OnLine");
      // ... and resubscribe
#ifdef DEBUG_PRINT
      Serial.println("Subscribe..");
#endif        
      //client.publish(statusTopic, "Window controller connected"); // Announce ourselves
      //client.subscribe(commandTopic);  // Listen for incoming commands
    } else {
#ifdef DEBUG_PRINT
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
#endif
      delay(5000);  // Wait 5 seconds before retrying
    }
  }
}

//void reconnect() {
//  // Loop until we're reconnected
//  while (!mqttClient.connected()) {
//    DEBUG_PRINT("Attempting MQTT connection...");
//    // Attempt to connect
//    // boolean connect (clientID, willTopic, willQoS, willRetain, willMessage)
//    if (mqttClient.connect("SneezyClient", (char *)TOPICBASE "State", 1, 0, "OFFLINE")) {
//      //if (mqttClient.connect("SleepyClient")) {
//      DEBUG_PRINTLN("connected");
//      // Once connected, publish an announcement...
//      Publish((char *)"State", (char *)"BOOTUP");
//      // ... and resubscribe
//      // Subscribe to enable bi-directional comms.
//      mqttClient.subscribe(TOPICBASE "Config/#");  // Allow bootup config fetching using MQTT persist flag!
//      //mqttClient.subscribe(TOPICBASE "Put/#");     // Send commands to this device, use Home/<device_name>/Get/# for responses.
//    } else {
//      DEBUG_PRINT("failed, rc=");
//      DEBUG_PRINT(mqttClient.state());
//      DEBUG_PRINTLN(" try again in 5 seconds");
//      // Wait 5 seconds before retrying
//      //digitalWrite(BLUE_PIN, HIGH);
//      delay(5000);
//      //digitalWrite(BLUE_PIN, LOW);
//    }
//  }
//}
