
// Protokollzeile mit Zeitstempel für die Fehlersuche bei der MP3-Einrichtung
static void dfLog(const String &s) {
  Serial.println("[AUDIO] " + String(millis()) + " ms: " + s);
}


// ---------------------------------------------------------------------------------------
// Befehle an den MP3-Player nur über diese Funktionen senden.
// Ohne ACK-Modus (siehe df_init) wartet die Bibliothek nach einem Befehl nur 10 ms. Der
// DFPlayer braucht aber deutlich länger, bis er den nächsten Befehl annimmt – bei
// "Stopp, Lautstärke, Play" innerhalb von 30 ms verwarf er oft das Play (Musik startete
// erst nach mehreren Klicks, 12.11.2026). Deshalb mindestens DF_CMD_GAP zwischen zwei
// Befehlen. Der Mutex verhindert, dass loop() und der Fernbedienungs-Task gleichzeitig senden.
#define DF_CMD_GAP 120                    // ms Mindestabstand zwischen zwei Player-Befehlen

static uint32_t dfLastCmd = 0;
static SemaphoreHandle_t dfMutex = NULL;

static bool dfBegin() {
  if (dfMutex == NULL) dfMutex = xSemaphoreCreateMutex();
  if (xSemaphoreTake(dfMutex, pdMS_TO_TICKS(1000)) != pdTRUE) return false;
  uint32_t d = millis() - dfLastCmd;
  if (d < DF_CMD_GAP) delay(DF_CMD_GAP - d);
  return true;
}

static void dfEnd() {
  dfLastCmd = millis();
  xSemaphoreGive(dfMutex);
}

void dfStop()                   { if (dfBegin()) { dfPlayer.stop();         dfEnd(); } }
void dfVolume(uint8_t v)        { if (dfBegin()) { dfPlayer.volume(v);      dfEnd(); } }
void dfPlay(int n)              { if (dfBegin()) { dfPlayer.play(n);        dfEnd(); } }
void dfEQ(uint8_t e)            { if (dfBegin()) { dfPlayer.EQ(e);          dfEnd(); } }
void dfOutputDevice(uint8_t d)  { if (dfBegin()) { dfPlayer.outputDevice(d); dfEnd(); } }
// ---------------------------------------------------------------------------------------


void df_init() {
  if (SUSE && !dfInit) {
    dfLog(F("Initializing mp3"));
    dfSerial.begin(9600, SERIAL_8N1, 18, 19);
    // Zeitlimit VOR dem ersten Befehl setzen (vorher erst nach begin(), Standard 500 ms je Befehl)
    dfPlayer.setTimeOut(300);
    dfLog(F("begin"));
    // OHNE ACK-Modus: Mit ACK wartet die Bibliothek (1.0.6) in sendStack() so lange, bis eine
    // Bestätigung (0x41) kommt. Schickt der Player stattdessen eine andere Meldung (Status,
    // Fehler, "Karte bereit"), bleibt _isAvailable gesetzt und die Warteschleife endet NIE –
    // so hing der Start (2026.11.11c–e, zuletzt bei readFileCounts). Ohne ACK wartet
    // sendStack() nie; nur die Abfragen warten, und zwar höchstens setTimeOut().
    if (!dfPlayer.begin(dfSerial, false, false)) {
      Serial.println("[AUDIO] DFPlayer unable to begin:");
      Serial.println("[AUDIO] 1.Please recheck the connection!");
      Serial.println("[AUDIO] 2.Please insert the SD card!");
    } else {
      // Antwortet der Player? Manche Player brauchen nach dem Einschalten 1–2 s,
      // deshalb bis zu ca. 2,5 s lang mehrfach fragen.
      int st = -1;
      unsigned long t0 = millis();
      while (st == -1 && millis() - t0 < 2500) {
        dfPlayer.readType();               // alte Meldung/Zeitüberschreitung verwerfen, sonst kehrt die Abfrage sofort zurück
        dfLog(F("readState"));
        st = dfPlayer.readState();
        if (st == -1) delay(300);
      }
      if (st == -1) dfLog(F("MP3-Player antwortet nicht – Befehle werden trotzdem gesendet"));
      else dfLog("Status " + String(st));
      dfStop();
      dfVolume(SVOL);
      dfEQ(EQ);
      dfOutputDevice(DFPLAYER_DEVICE_SD);
      delay(200);                          // der Player braucht nach der Gerätewahl einen Moment
      fileCounts = -1;
      if (st != -1) {
        dfPlayer.readType();
        // Titelanzahl auf der SD-Karte (früher wurde fälschlich die Nummer des aktuellen Titels verwendet)
        dfLog(F("readFileCounts"));
        fileCounts = dfPlayer.readFileCounts();
        if (fileCounts < 1) {              // zweiter Versuch, falls eine alte Meldung dazwischenkam
          dfPlayer.readType();
          fileCounts = dfPlayer.readFileCounts();
        }
      }
      dfLog("Found " + String(fileCounts) + " files.");

      /*if (fileCounts == -1) {
        Serial.print("[AUDIO] ");
        printDetail(dfPlayer.readType(), dfPlayer.read());
        }*/

      if (fileCounts < 1) {
        Serial.println("[AUDIO] Default number of files is set.");
        fileCounts = NUMBER_SOUNDTRACKS;
      }
      dfInit = true;
    }
  }
}


int mp3StrPars(const String &s) {
  if (s == "")return 0;
  if (s.indexOf(",") == -1) return s.toInt();
  else {
    int c = 0;
    for (int i = 0; i < s.length(); i++) {
      if (s[i] == ',') c++;
    }
    static int rnd = 0;
    if (SRND) {
      randomSeed(millis() / 1000);
      rnd = random(1, c + 2);
    } else {
      if (rnd < c + 1) ++rnd;
      else rnd = 1;
    }
    c = 1;
    for (int i = 0; i < s.length(); i++) {
      if (c == rnd) return s.substring(i, s.indexOf(",", i)).toInt();
      if (s[i] == ',') c++;
    }
  }
  return 0;
}


/*
  void printDetail(uint8_t type, int value) {
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerUSBInserted:
      Serial.println("USB Inserted!");
      break;
    case DFPlayerUSBRemoved:
      Serial.println("USB Removed!");
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }

  }*/
