
void weatherRun() {
  if (WTHR && VTIN && VTOUT) {

    static uint32_t pause = 10000UL;
    static uint32_t weatherTimer = 0;
    if (millis() - weatherTimer >= pause) {
      weatherTimer = millis();

      if (WiFi.status() == WL_CONNECTED) {
        String WID(crm.var(F("WID")));
        if (WID != "" && WID != "null") {
          Serial.println(F("\n[HTTP] Weather start..."));
          const String &searchObject(F("fact__temp_size_s"));
          uint8_t objSize = 10;

          String lat(WID.substring(0, WID.indexOf(",")));
          lat.trim();
          String lon(WID.substring(WID.indexOf(",") + 1));
          lon.trim();
          //Serial.printf("lat: |%s|\t\tlon: |%s|\n", lat, lon);
          String u(F("https://meteum.ai/weather/?lat="));
          u += lat;
          u += F("&lon=");
          u += lon;
          
          HTTPClient h;
          h.begin(u);
          h.setUserAgent(F("Mozilla/5.0 (X11; Linux i686; rv:90.0) Gecko/20100101 Firefox/90.0"));
          h.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
          int hCode = h.GET();

          Serial.print(F("[HTTP] Request code: "));
          Serial.print(hCode);
          Serial.print(F("\t"));
          Serial.println(h.errorToString(hCode).c_str());

          if (hCode == 200) {
            pause = 600000UL;   // 10 min
            int len = h.getSize();
            uint8_t buff[256] = {0};
            String oldSTR = String();
            bool ObjFound = false;
            uint8_t count = 0;
            uint32_t timer = millis();
            WiFiClient *stream = h.getStreamPtr();
            while (h.connected() && (len > 0 || len == -1) && count < objSize && millis() - timer < 30000UL) {
              size_t size = stream->available();
              if (size) {
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                //Serial.write(buff, c);
                String newSTR = String();
                for (size_t i = 0; i < c; i++) {
                  newSTR += (char)buff[i];
                }
                if (ObjFound) {
                  oldSTR += newSTR;
                  count++;
                } else {
                  if ((oldSTR + newSTR).indexOf(searchObject) != -1) {
                    ObjFound = true;
                    oldSTR += newSTR;
                  } else oldSTR = newSTR;
                }
                if (len > 0) len -= c;
              }
              delay(1);
            }

            Serial.println(F("-["));
            Serial.println(oldSTR);
            Serial.println(F("]-"));

            parserYa(oldSTR);

            if (crm.webConnStatus()) {
              crm.webUpdate("WR", weatherString());
            }

          } else pause = 120000UL;  // 2 min
          h.end();
          Serial.println(F("[HTTP] Weather end"));
        } else {
          if (crm.webConnStatus()) {
            crm.webUpdate("WR", F("Weather location id not set!"));
          }
          Serial.println(F("[HTTP] Weather location id not set!"));
          pause = 10000UL;     // 10 sec
        }
      } else pause = 30000UL;     // 30 sec

    }
  }
}



void parserYa(const String &s) {
  //ветер
  int a = s.indexOf("\"wind-speed");
  String subs = s.substring(s.indexOf(">", a) + 1, s.indexOf("<", a));
  subs.replace(",", ".");
  weather.W = subs.toFloat();

  //дождь
  weather.R = ((s.indexOf("Rain") != -1 || s.indexOf("rain") != -1) ? true : false);
}


String weatherString() {
  String s("");
  s += F("Rain: ");
  s += (weather.R ? F("Yes") : F("No"));
  s += F("<br>Wind: ");
  s += String(weather.W, 1);
  s+= F(" m/s");
  return s;
}
