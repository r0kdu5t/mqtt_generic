void Publish(char *Topic, char *Message)
{
  char TopicBase[80] = TOPIC_BASE;

  strcat(TopicBase, Topic);
  client.publish(TopicBase, Message);
}

void PublishFloat(char *Topic, float Value)
{
  char TopicBase[80] = TOPIC_BASE;
  char Message[10] = "NULL";

  if (!isnan(Value))
    dtostrf(Value, 5, 2, Message);

  strcat(TopicBase, Topic);
  //client.publish(TopicBase, Message);

  if (client.publish(TopicBase, Message))
  {
    patWatchdog();  // Only pat the watchdog if we successfully published to MQTT
#ifdef DEBUG_PRINT
    Serial.println("Pat the Dog!");
#endif
  }
  // The interval timer is updated inside patWatchdog()
}
