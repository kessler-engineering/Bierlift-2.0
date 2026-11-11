
// Wetter für Night Cooling über die Open-Meteo-API (https://open-meteo.com).
// Kostenlos für nicht-kommerzielle Nutzung, kein API-Schlüssel, Antwort ~300 Byte JSON.
// Früher wurde die HTML-Seite von meteum.ai ausgelesen (Suche nach der CSS-Klasse
// "AppFact_wrap__"). Nach einem Umbau der Seite kam dort nichts mehr an -> immer
// "Rain: No, Wind 0.0", d. h. Night Cooling hat das Wetter faktisch ignoriert.
//
// Koordinaten wie bisher im Feld WID: "lat, lon" (z. B. "51.28, 9.55").

#define WEATHER_MAX_AGE 3600000UL         // Wetterdaten älter als 1 h gelten als ungültig


void weatherTask(void *pvParameters) {
  for (;;) {
    weatherRun();
    vTaskDelay(1000);
  }
}


// true = es liegen aktuelle Wetterdaten vor
bool weatherValid() {
  return weather.ok && millis() - weather.t < WEATHER_MAX_AGE;
}


void weatherRun() {
  if (WTHR && VTIN && VTOUT) {

    static uint32_t pause = 10000UL;
    static uint32_t weatherTimer = 0;
    if (millis() - weatherTimer >= pause) {
      weatherTimer = millis();

      if (WiFi.status() == WL_CONNECTED) {
        String WID(crm.var(F("WID")));
        int comma = WID.indexOf(",");
        if (WID != "" && WID != "null" && comma > 0) {
          String lat(WID.substring(0, comma));
          lat.trim();
          String lon(WID.substring(comma + 1));
          lon.trim();

          // HTTP statt HTTPS: spart ca. 40 KB RAM für TLS, es werden keine
          // vertraulichen Daten übertragen.
          String u(F("http://api.open-meteo.com/v1/forecast?latitude="));
          u += lat;
          u += F("&longitude=");
          u += lon;
          u += F("&current=precipitation,wind_speed_10m&wind_speed_unit=ms");

          Serial.println(F("\n[WEATHER] Request Open-Meteo..."));
          HTTPClient h;
          h.useHTTP10(true);             // kein Chunked-Encoding -> direkt aus dem Stream parsen
          h.setTimeout(10000);
          h.begin(u);
          h.setUserAgent(F(BL_NAME "/" CRM_VER));
          int hCode = h.GET();
          Serial.printf("[WEATHER] HTTP %d\n", hCode);

          if (hCode == 200) {
            StaticJsonDocument<128> filter;
            filter["current"]["precipitation"] = true;
            filter["current"]["wind_speed_10m"] = true;
            DynamicJsonDocument doc(256);
            DeserializationError err = deserializeJson(doc, h.getStream(), DeserializationOption::Filter(filter));
            JsonVariant cur = doc["current"];
            if (!err && !cur["precipitation"].isNull() && !cur["wind_speed_10m"].isNull()) {
              float p = cur["precipitation"].as<float>();   // mm im letzten 15-min-Intervall
              weather.W = cur["wind_speed_10m"].as<float>();
              weather.R = p > 0.0f;
              weather.ok = true;
              weather.t = millis();
              pause = 1200000UL;         // 20 min
              Serial.printf("[WEATHER] Rain %.2f mm, wind %.1f m/s\n", p, weather.W);
            } else {
              Serial.println(String(F("[WEATHER] Invalid response: ")) + err.c_str());
              pause = 300000UL;          // 5 min
            }
          } else pause = 300000UL;       // 5 min
          h.end();

          if (crm.webConnStatus()) crm.webUpdate("WR", weatherString());
        } else {
          if (crm.webConnStatus()) crm.webUpdate("WR", lng(L_COORD_NOT_SET));
          Serial.println(F("[WEATHER] Coordinates not set!"));
          pause = 10000UL;               // 10 s
        }
      } else pause = 30000UL;            // 30 s
    }
  }
}


String weatherString() {
  if (!weatherValid()) return F("n/a");
  String s("");
  s += lng(L_RAIN);
  s += (weather.R ? lng(L_YES) : lng(L_NO));
  s += "<br>" + lng(L_WIND);
  s += String(weather.W, 1);
  s += F(" m/s");
  return s;
}
