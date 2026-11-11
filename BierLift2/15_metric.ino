//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_MaxFragmentLength/BearSSL_MaxFragmentLength.ino


void metricRun(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  for (;;) {
    uint32_t t = checkUpdate();
    vTaskDelay(t);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //Test: Stack-Reserve des Tasks melden
      Serial.println("[HEAP] UpdateChecker: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


// Button "Nach Updates suchen" im Web-Interface. Wird aus loop() aufgerufen –
// die HTTPS-Abfrage läuft deshalb in einem kurzlebigen eigenen Task.
void checkUpdateWeb() {
  crm.webNotif("i", lng(L_START_CHECK_UPDATE), 3);
  xTaskCreatePinnedToCore(checkUpdateWebTask, "UpdateCheckWeb", 8000, NULL, 1, NULL, tskNO_AFFINITY);
}

void checkUpdateWebTask(void *pvParameters) {
  checkUpdate();
  if (newVerSt() < 2) crm.webNotif("i", lng(L_NOT_UPDATE), 3);
  vTaskDelete(NULL);
}


// Neueste Version aus den GitHub-Releases (FW_UPDATE_REPO in 00_sett.h).
// Abgefragt wird nur der Tag-Name des neuesten Releases, z. B. "v2025.11.11r".
// Ohne Token: GitHub erlaubt 60 Anfragen pro Stunde und IP, geprüft wird alle 12 h.
// Früher: Blynk-Datastream V100 mit fest eingebautem Geräte-Token (das Token erlaubte
// auch Schreiben – jeder hätte allen Geräten ein "Update" melden können).
String lastVerTag;

uint32_t checkUpdate() {
  static volatile bool checkMutex = false;
  uint32_t delayInterval = 10000;
  if (!checkMutex && WiFi.status() == WL_CONNECTED) {
    checkMutex = true;
    Serial.println("\n[UDT_FW] Check update firmware...");

    HTTPClient h;
    WiFiClientSecure *client = new WiFiClientSecure;
    client->setInsecure();
    h.useHTTP10(true);                       // kein Chunked-Encoding -> direkt aus dem Stream parsen
    h.begin(*client, F("https://api.github.com/repos/" FW_UPDATE_REPO "/releases/latest"));
    h.setUserAgent(F(BL_NAME "/" CRM_VER));  // GitHub verlangt einen User-Agent
    h.addHeader(F("Accept"), F("application/vnd.github+json"));
    int httpResponseCode = h.GET();

    Serial.println("[UDT_FW] Response code: " + String(httpResponseCode));
    if (httpResponseCode == 200) {
      delayInterval = 43200000;              // 12 h
      // Die Antwort ist mehrere KB groß (Release-Text, Assets) – nur tag_name einlesen.
      StaticJsonDocument<64> filter;
      filter["tag_name"] = true;
      DynamicJsonDocument doc(256);
      DeserializationError err = deserializeJson(doc, h.getStream(), DeserializationOption::Filter(filter));
      if (!err) {
        String tag = doc["tag_name"] | "";
        long v = verToNum(tag);
        Serial.println("[UDT_FW] Latest release: " + tag);
        if (v > 0) {
          lastVER = v;
          lastVerTag = tag;
        }
        if (newVerSt() == 2) udtNotifSend();
      } else Serial.println(String(F("[UDT_FW] JSON error: ")) + err.c_str());
    } else if (httpResponseCode == 404) {
      delayInterval = 43200000;              // noch kein Release veröffentlicht
    } else delayInterval = 900000;           // 15 min
    h.end();
    delete client;
    checkMutex = false;
  } else delayInterval = 60000;              // 1 min
  return delayInterval;
}


// Versionsstring -> JJJJMMTT, für beide Schemata:
// "2023.02.12a" -> 20230212, "25.06.27r" -> 20250627 (zweistelliges Jahr = 20xx).
// Früher wurden nur die Punkte entfernt: "25.06.27r" (250627) galt dann als älter
// als "2023.02.12a" (20230212). Buchstaben am Ende werden ignoriert. 0 = unlesbar.
long verToNum(const String &s) {
  long part[3] = {0, 0, 0};
  int n = 0;
  bool digit = false;
  for (unsigned int i = 0; i < s.length() && n < 3; i++) {
    char c = s[i];
    if (c >= '0' && c <= '9') {
      part[n] = part[n] * 10 + (c - '0');
      digit = true;
    } else if (c == '.' && digit) {
      n++;
      digit = false;
    } else if (digit) break;   // Suffix wie "a"/"r"
  }
  if (part[0] == 0 || part[1] < 1 || part[1] > 12 || part[2] < 1 || part[2] > 31) return 0;
  if (part[0] < 100) part[0] += 2000;
  return part[0] * 10000L + part[1] * 100L + part[2];
}


uint8_t newVerSt() {
  long cur = verToNum(CRM_VER);
  if (lastVER == cur) return 1;
  else if (lastVER > cur) return 2;
  else return 0;
}


void udtNotifSend() {
  Serial.println("[UDT_FW] " + lng(L_NEWVER) + lastVerTag);
  crm.webNotif("Green", lng(L_NEWVER) + lastVerTag, 5);
}
