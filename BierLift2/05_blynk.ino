//Manual    https://docs.blynk.cc/#blynk-firmware-connection-management-blynkconnect
//API GET   https://smdx.ru/blog/blynk/blynk-api
//GET VAL   http://blynk-cloud.com/468Yt0d5FX3tDLnp1KGPPgi7iiHa93aO/get/V0
//UPDATE VAL    http://blynk-cloud.com/468Yt0d5FX3tDLnp1KGPPgi7iiHa93aO/update/V0?value=0
//              http://blynk-cloud.com/468Yt0d5FX3tDLnp1KGPPgi7iiHa93aO/update/V0?value=1


void blynk_start (void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  bool blynk_init = false;
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      xSemaphoreTake(wifi_mutex, portMAX_DELAY);
      if (!blynk_init) {
        Serial.println("[BLYNK] Initializing Blynk...");
        blynk_init = true;
        Blynk.setTemplateID(crm.var("BTID"));
        String auth_API_blynk = crm.var("BAPI");
        char *cstr = new char[auth_API_blynk.length() + 1];
        strcpy(cstr, auth_API_blynk.c_str());
        Blynk.config(cstr);
        //Blynk.config(cstr, "crims.noip.me", 48080);
      } else {
        Blynk.run();
        blynkConnSt = Blynk.connected();
        if (BUSE) crm.webUpdate("BST", (blynkConnSt ? ("<span style='color:#0f0;'>" +
                                          lng(L_CONNECTED) + "</span>") : ("<span style='color:#f00;'>" +
                                              lng(L_DISCONNECTED) + "</span>")));
        blynk_action();
      }
      xSemaphoreGive(wifi_mutex);
    }
    vTaskDelay(700);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //TEST
      Serial.println("[HEAP] Blynk: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}



BLYNK_WRITE(V0) {
  blynkAction = param.asInt();
  blynkActionStatus = true;
  Serial.println("[BLYNK] Incoming data V0: " + String(blynkAction));
}


void blynk_action() {
  if (blynkActionStatus) {
    blynkActionStatus = false;
    releyAction(blynkAction);
  }
}


BLYNK_CONNECTED() {
  //Blynk.syncAll();
  //Blynk.syncVirtual(V0);
  Blynk.virtualWrite(V0, 0);
}
