//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_MaxFragmentLength/BearSSL_MaxFragmentLength.ino


void metricRun(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  for (;;) {
    uint32_t t = checkUpdate();
    vTaskDelay(t);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //TEST
      Serial.println("[HEAP] UpdateChecker: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


void checkUpdateWeb() {
  crm.webNotif("i", lng(L_START_CHECK_UPDATE), 3);
  checkUpdate();
  if (newVerSt() < 2) {
    crm.webNotif("i", lng(L_NOT_UPDATE), 3);
  }
}

uint32_t checkUpdate() {
  static bool checkMutex = false;
  uint32_t delayInterval = 10000;
  if (!checkMutex && WiFi.status() == WL_CONNECTED) {
    checkMutex = true;
    Serial.println("\n[UDT_FW] Check update firmware...");
    /*StaticJsonDocument<150> doc;
      String ipJson = getMyIP(), ip, country;
      DeserializationError error = deserializeJson(doc, ipJson);
      ipJson = String();
      if (error) {
      ip = country = "n/a";
      } else {
      const char* _ip = doc["ip"];
      const char* _cc = doc["cc"];
      ip = _ip;
      country = _cc;
      HTTPClient h;
      WiFiClient client;
      h.begin(client, "http://crims.noip.me:40080/bierlift/api");
      h.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String postData = "key=Kdr94_234ldl_sDfd&id=" + crm.getID() + "&ip=" + ip + "&country=" + country + "&uptime=" + crm.upTime(true) + "&ver=" + CRM_VER;
      int httpResponseCode = h.POST(postData);*/

    HTTPClient h;
    WiFiClientSecure *client = new WiFiClientSecure;
    client->setInsecure();
    h.begin(*client, "https://blynk.cloud/external/api/get?token=b-Mdp6Jjr4xmDM5jjAiV_pM7LhpS-bpn&V100");
    int httpResponseCode = h.GET();

    Serial.println("[UDT_FW] Response code: " + String(httpResponseCode));
    if (httpResponseCode == 200) {
      delayInterval = 43200000; 						//12ч
      /*String ResponseStr = h.getString();
        error = deserializeJson(doc, ResponseStr);
        if (error) {
        Serial.println("[UDT_FW] " + ResponseStr);
        crm.webNotif("Red", "UpdateChecker. " + ResponseStr, 4);
        } else {
        const char* _lastVER = doc["lastVersion"];
        lastVER = _lastVER;
        if (newVerSt() == 2) udtNotifSend();
        }
        ResponseStr = String();*/
      { 
        String lv = h.getString();
        lv.replace(".", "");
        lastVER = lv.toDouble();
      }
      if (newVerSt() == 2) udtNotifSend();
    } else delayInterval = 900000; //15 мин
    h.end();
    delete client;
    //Serial.println("[UDT_FW] End...\n");
    checkMutex = false;
  } else delayInterval = 60000;  //1 мин
  return delayInterval;
}


uint8_t newVerSt() {
  String C = CRM_VER;
  C.replace(".", "");
  if (lastVER == C.toDouble()) return 1;
  else if (lastVER > C.toDouble()) return 2;
  else return 0;
}


String getMyIP() {
  String s;
  WiFiClientSecure *client = new WiFiClientSecure;
  client->setInsecure();
  HTTPClient h;
  h.begin(*client, "https://api.myip.com/");        //IP + страна
  int c = h.GET();
  if (c == HTTP_CODE_OK || c == HTTP_CODE_MOVED_PERMANENTLY) s = h.getString();
  if (!s.startsWith("{") || !s.endsWith("}")) s = "{}";
  //Serial.println(c);
  //Serial.println(s);
  h.end();
  return s;
}

void udtNotifSend() {
  Serial.println("[UDT_FW] " + lng(L_NEWVER));
  crm.webNotif("Green", lng(L_NEWVER), 5);
}
