/**
   Pulse the hardware watchdog timer pin to reset it
*/
void patWatchdog()
{
  digitalWrite(WATCHDOG_PIN, HIGH);
  delay(WATCHDOG_PULSE_LENGTH);
  digitalWrite(WATCHDOG_PIN, LOW);
  watchdogLastResetTime = millis();
}

