//https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/

void timeInit() {
  timeTicTicker.attach_ms(1000, timeTic);
  timeSyncTicker.once_ms(100, timeSync);
}



// Läuft als Ticker-Callback (esp_timer-Task): hier nicht blockieren.
// Früher stand hier delay(3000), das hielt alle Ticker (MQTT-Reconnect, Night Cooling) auf.
void timeSync() {
  uint32_t pause = 86400000UL;
  auto s = WiFi.status();
  if (s == WL_CONNECTED) {
    struct tm now;
    if (!getLTime(&now)) {
      Serial.println(F("[CLOCK] NTP sync"));
      if (DSTEU) configTzTime(tzStringEU().c_str(), "pool.ntp.org", "time.nist.gov");
      else configTime(TZ * 60, 0, "pool.ntp.org", "time.nist.gov");
      pause = 7000UL;                   // in 7 s prüfen, ob die Zeit da ist
    }
    // Zeit vorhanden: SNTP gleicht selbstständig nach, nächste Prüfung in 24 h
  } else pause = 5000UL;
  timeSyncTicker.once_ms(pause, timeSync);
}


// POSIX-Zeitzone mit EU-Sommerzeitregel (letzter Sonntag März/Oktober, 01:00 UTC).
// TZ = Abstand der Winterzeit zu UTC in Minuten (Deutschland: 60).
String tzStringEU() {
  int m = abs(TZ);
  char off[12];
  // POSIX: Vorzeichen umgekehrt (UTC+1 -> "-1")
  if (m % 60) snprintf(off, sizeof(off), "%c%d:%02d", (TZ > 0 ? '-' : '+'), m / 60, m % 60);
  else snprintf(off, sizeof(off), "%c%d", (TZ > 0 ? '-' : '+'), m / 60);
  int startH = 1 + TZ / 60;             // 01:00 UTC in Winterzeit
  int endH = 2 + TZ / 60;               // 01:00 UTC in Sommerzeit
  char tz[48];
  snprintf(tz, sizeof(tz), "STD%sDST,M3.5.0/%d,M10.5.0/%d", off, startH, endH);
  return String(tz);
}


void applyTimeZone() {
  if (DSTEU) {
    setenv("TZ", tzStringEU().c_str(), 1);
    tzset();
  } else setTimeZone(-60 * TZ);
}


void timeTic() {
  if (VTIN && VTOUT && getLTime(&TD)) {
    if (nCoolStatus) {
      // Schließen (Lift herunterfahren)
      if (checkTimePeriod(TOPN, TCSE) == false ||
          Tin == -127 || Tout == -127 ||
          Tin <= Tout ||
          (WTHR ? (!weatherValid() || weather.R || weather.W > WSPD) : false)) {   // ohne aktuelle Wetterdaten lieber schließen
        nCoolStatus = false;
        effMute = true;
        releyAction(-1);
      }
    } else {
      // Öffnen (Lift kurz anheben)
      if (checkTimePeriod(TOPN, TCSE) &&
          Tin != -127 && Tout != -127 &&
          (Tin - Tout >= TGIST) &&
          (WTHR ? (weatherValid() && weather.R == false && weather.W <= WSPD) : true)) {   // nur mit aktuellen Wetterdaten öffnen
        if (lift_position) nCoolStatus = true;
        else {
          effMute = true;
          nCoolStatus = true;
          releyAction(1);
          nightCollTicker.once_ms(TMOVE, releyAction, 0);
        }
      }
    }


    if (crm.webConnStatus()) {
      crm.webUpdate("TM", curTime());
      //crm.webUpdate("DT", curData());
    }
  }
}


bool getLTime(struct tm * info) {
  time_t now;
  time(&now);
  localtime_r(&now, info);
  if (info->tm_year > 100) {
    return true;
  }
  return false;
}



void setTimeZone(long offset) { //Sekunden
  int daylight = 0; //Winterzeit (ohne Sommerzeit-Verschiebung)
  char cst[17] = {0};
  char cdt[17] = "DST";
  char tz[33] = {0};

  if (offset % 3600) {
    sprintf(cst, "UTC%ld:%02u:%02u", offset / 3600, abs((offset % 3600) / 60), abs(offset % 60));
  } else {
    sprintf(cst, "UTC%ld", offset / 3600);
  }
  if (daylight != 3600) {
    long tz_dst = offset - daylight;
    if (tz_dst % 3600) {
      sprintf(cdt, "DST%ld:%02u:%02u", tz_dst / 3600, abs((tz_dst % 3600) / 60), abs(tz_dst % 60));
    } else {
      sprintf(cdt, "DST%ld", tz_dst / 3600);
    }
  }
  sprintf(tz, "%s%s", cst, cdt);
  setenv("TZ", tz, 1);
  tzset();
}


String curTime() {
  String s(TD.tm_hour);
  s += ":";
  if (TD.tm_min < 10) s += "0";
  s += TD.tm_min;
  s += ":";
  if (TD.tm_sec < 10) s += "0";
  s += TD.tm_sec;
  return s;
}


bool checkTimePeriod(uint16_t s, uint16_t e) {
  int HM = TD.tm_hour * 60 + TD.tm_min;
  if (s <= e) {
    if (HM >= s && HM < e) return true;
    else return false;
  } else {
    if ((HM >= max(s, (uint16_t)0) && HM <= 1439) || (HM >= 0 && HM < max(e, (uint16_t)0))) return true;
    else return false;
  }
}


/*String curData() {
  String d(weekDay());
  d += F(", ");
  d += TD.tm_mday;
  d += F(" ");
  d += fullMonth();
  d += F(" ");
  d += (TD.tm_year + 1900);
  return d;
  }*/


uint32_t timeStrToSec(const String & t) {
  uint32_t s;
  s = (t.substring(0, 2).toInt() * 3600);
  s += (t.substring(3, 5).toInt() * 60);
  s += t.substring(6).toInt();
  return s;
}


uint32_t timeStrToMin(const String & t) {
  uint32_t m;
  m = (t.substring(0, 2).toInt() * 60);
  m += t.substring(3, 5).toInt();
  return m;
}



/*
  TD.tm_sec           // Sekunden 0 bis 61
  TD.tm_min           // Minuten 0 bis 59
  TD.tm_hour          // Stunden 0 bis 23

  TD.tm_mday          // Tag im Monat 1 bis 31
  TD.tm_mon + 1       // Monat 0 bis 11
  TD.tm_year + 1900   // Jahre seit 1900
  TD.tm_wday          // Wochentag, Sonntag = 0
  TD.tm_yday          // Tag im Jahr seit 1. Januar
  TD.tm_isdst         // Sommerzeit-Kennzeichen
*/
