//https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/BearSSL_MaxFragmentLength/BearSSL_MaxFragmentLength.ino


void metricRun(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  // Erste Prüfung direkt nach dem WLAN-Verbinden, solange Telegram und Blynk noch warten
  // (udtBootCheck). Neben deren TLS-Verbindungen reicht der Speicher für eine dritte nicht:
  // 12g stürzte dabei ab, 12h/13a scheiterten mit "BIGNUM - Memory allocation failed"
  // (ESP .64, 13.11.2026). Spätere Prüfungen laufen nur bei genug freiem Speicher.
  uint32_t t0 = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - t0 < 60000) vTaskDelay(500);
  vTaskDelay(1000);
  uint32_t t = checkUpdate();
  udtBootCheck = false;
  for (;;) {
    vTaskDelay(t);
    t = checkUpdate();
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

// Mindestspeicher für die HTTPS-Abfrage (siehe checkUpdate()).
// Gemessen 13.11.2026 auf ESP .64 (Telegram + Blynk aktiv): frei 109900, größter Block 33772
// -> TLS scheiterte (-1). Mit 45 KB am Stück wird die Prüfung dort übersprungen statt einen
// aussichtslosen Versuch zu starten, der kurzzeitig den ganzen Speicher belegt.
// 13.11. erneut: frei 118372, Block 48396 -> TLS scheiterte trotzdem -> Schwelle 60 KB; die
// eigentliche Prüfung läuft seit 13b beim Start vor Telegram/Blynk (siehe metricRun()).
#define UDT_MIN_FREE_HEAP 70000
#define UDT_MIN_BLOCK     60000

uint32_t checkUpdate() {
  static volatile bool checkMutex = false;
  uint32_t delayInterval = 10000;
  if (!checkMutex && WiFi.status() == WL_CONNECTED) {
    // Eine TLS-Verbindung braucht ≈40–50 KB. Laufen schon Telegram und Blynk, reicht der Speicher
    // oft nicht – dann schlugen andere Speicheranforderungen fehl und der ESP startete neu
    // (12.11.2026, ESP .64, alle ≈2 min). Zu wenig frei -> Prüfung auslassen, in 15 min erneut.
    size_t freeHeap = ESP.getFreeHeap();
    size_t maxBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
    Serial.printf("\n[UDT_FW] Free heap %u, largest block %u\n", freeHeap, maxBlock);
    if (freeHeap < UDT_MIN_FREE_HEAP || maxBlock < UDT_MIN_BLOCK) {
      Serial.println(F("[UDT_FW] Not enough memory for TLS - check skipped"));
      return 900000;                         // 15 min
    }
    checkMutex = true;
    Serial.println("[UDT_FW] Check update firmware...");

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
