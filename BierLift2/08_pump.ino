
void pump_action(bool action) {
  if (USEPM) {
    digitalWrite(PUMP_PIN, (action ? pumpStatus = true : pumpStatus = false));
    Serial.println("Pump is " + (pumpStatus ? String("start") : String("stop")));
    crm.webUpdate("STATUS", workState());
    crm.webUpdate("PUMP", (pumpStatus ? lng(L_STOP) : lng(L_PUSK)));
    crm.webNotif(action ? "Red" : "Green", pumpStatus ? lng(L_PUMP_IS_START) : lng(L_PUMP_IS_STOP), 5);
    tgNotif(pumpStatus ? lng(L_PUMP_IS_START) : lng(L_PUMP_IS_STOP));
    mqttSend("s", pumpStatus ? lng(L_PUMP_IS_START) : lng(L_PUMP_IS_STOP));
  }
}


void task_pump(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  uint32_t pompTimer = 0;
  bool pumpDelay = false;
  for (;;) {
    if (waterVol && !pumpStatus) {
      pump_action(1);
      pumpDelay = true;
    } else if (!waterVol && pumpStatus) {
      if (pumpDelay) {
        pompTimer = millis();
        pumpDelay = false;
      } else if (millis() - pompTimer >= DELAYPM) pump_action(0);
    }

    vTaskDelay(1000);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //TEST
      Serial.println("[HEAP] Pump: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}
