//https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/

void timeInit() {
  timeTicTicker.attach_ms(1000, timeTic);
  timeSyncTicker.once_ms(100, timeSync);
}



void timeSync() {
  uint32_t pause = 86400000UL;
  auto s = WiFi.status();
  if (s == WL_CONNECTED) {
    Serial.println(F("[CLOCK] Синхронизация с NTP"));
    configTime(TZ * 60, 0, "pool.ntp.org", "time.nist.gov");
    delay(3000);
    if (TD.tm_year < 120) pause = 7000UL;
  } else pause = 5000UL;
  timeSyncTicker.once_ms(pause, timeSync);
}


void timeTic() {
  if (VTIN && VTOUT && getLTime(&TD)) {
    if (nCoolStatus) {
      // Закрытие
      if (checkTimePeriod(TOPN, TCSE) == false ||
          Tin == -127 || Tout == -127 ||
          Tin <= Tout ||
          (WTHR ? (weather.R || weather.W > WSPD) : false)) {
        nCoolStatus = false;
        effMute = true;
        releyAction(-1);
      }
    } else {
      // Открытие
      if (checkTimePeriod(TOPN, TCSE) &&
          Tin != -127 && Tout != -127 &&
          (Tin - Tout >= TGIST) &&
          (WTHR ? (weather.R == false && weather.W <= WSPD) : true)) {
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



void setTimeZone(long offset) { //sec
  int daylight = 0; //зимнее время
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
  TD.tm_sec           // seconds of minutes from 0 to 61
  TD.tm_min           // minutes of hour from 0 to 59
  TD.tm_hour          // hours of day from 0 to 24

  TD.tm_mday          // day of month from 1 to 31
  TD.tm_mon + 1       // month of year from 0 to 11
  TD.tm_year + 1900   // year since 1900
  TD.tm_wday          // days since sunday
  TD.tm_yday          // days since January 1st
  TD.tm_isdst         // hours of daylight savings time
*/
