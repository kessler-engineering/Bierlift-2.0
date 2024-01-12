
void df_init() {
  if (SUSE && !dfInit) {
    Serial.println("[AUDIO] Initializing mp3");
    dfSerial.begin(9600, SERIAL_8N1, 18, 19);
    if (!dfPlayer.begin(dfSerial, true, false)) {
      Serial.println("[AUDIO] DFPlayer unable to begin:");
      Serial.println("[AUDIO] 1.Please recheck the connection!");
      Serial.println("[AUDIO] 2.Please insert the SD card!");
    } else {
      dfPlayer.setTimeOut(100);   //Set serial communictaion time out 500ms
      dfPlayer.stop();
      dfPlayer.volume(SVOL);
      dfPlayer.EQ(EQ);
      dfPlayer.outputDevice(DFPLAYER_DEVICE_SD);
      dfPlayer.readState();
      dfPlayer.readFileCounts();
      fileCounts = dfPlayer.readCurrentFileNumber();
      Serial.println("[AUDIO] Found " + String(fileCounts) + " files.");

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
