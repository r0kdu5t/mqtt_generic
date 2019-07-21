/**
   Attempt connection to the MQTT broker, and try repeatedly until it succeeds
*/
void reconnect() {
  // Generate a unique MQTT client ID from our IP address
  char clientBuffer[50];
  String clientString = client_id + String(Ethernet.localIP());
#ifdef DEBUG_PRINT
    Serial.println(clientString);
#endif  
  clientString.toCharArray(clientBuffer, clientString.length() + 1);

  while (!client.connected()) {
#ifdef DEBUG_PRINT
    Serial.print("Attempting MQTT connection...");
#endif
    if (client.connect(clientBuffer)) {
#ifdef DEBUG_PRINT
      Serial.println("connected");
#endif
      client.publish(statusTopic, "Window controller connected"); // Announce ourselves
      client.subscribe(commandTopic);  // Listen for incoming commands
    } else {
#ifdef DEBUG_PRINT
      //Serial.print("failed, rc=");
      //Serial.print(client.state());
      //Serial.println(" try again in 5 seconds");
#endif
      delay(5000);  // Wait 5 seconds before retrying
    }
  }
}
