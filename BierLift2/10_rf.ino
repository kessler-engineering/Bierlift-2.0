
void rfControl(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  RCSwitch rfReceiver = RCSwitch();
  uint32_t rfProtectTimer = 0;
  pinMode(RF_PIN, INPUT);
  rfReceiver.enableReceive(RF_PIN);
  for (;;) {
    if (rfReceiver.available()) {
      lastReceivedCode = rfReceiver.getReceivedValue();
      crm.webUpdate("LCODE", String(lastReceivedCode));

      Serial.print("[RF] Received: ");
      Serial.println(lastReceivedCode);
      /*Serial.print("\t");
        Serial.print(rfReceiver.getReceivedBitlength());
        Serial.print("\t");
        Serial.print(rfReceiver.getReceivedDelay());
        Serial.print("\t");
        Serial.println(rfReceiver.getReceivedProtocol());*/

      if (millis() - rfProtectTimer > 500) {
        if (lastReceivedCode == CODE1) {
          rfAction(crm.var("CODE1A").toInt());
        }

        if (lastReceivedCode == CODE2) {
          rfAction(crm.var("CODE2A").toInt());
        }

        if (lastReceivedCode == CODE3) {
          rfAction(crm.var("CODE3A").toInt());
        }

        if (lastReceivedCode == CODE4) {
          rfAction(crm.var("CODE4A").toInt());
        }
      }

      rfProtectTimer = millis();
      rfReceiver.resetAvailable();
    }

    vTaskDelay(500);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //Test: Stack-Reserve des Tasks melden
      Serial.println("[HEAP] RF: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


void rfAction(int a) {
  switch (a) {
    //hoch / stopp / runter im Wechsel
    case 1: {
        releyAction(LIFT_CMD_TOGGLE);   // Wechsel wird in loop() ausgewertet
      } break;
    //hoch
    case 2: {
        releyAction (1);
      } break;
    //runter
    case 3: {
        releyAction (-1);
      } break;
    //Stopp
    case 4: {
        releyAction (0);
      } break;
    //Musik lauter
    case 5: {
        if (SVOL < 31) {
          SVOL++;
          dfPlayer.volume(SVOL);
          crm.webUpdate("SVOL", String(SVOL), true);
        }
      } break;
    //Musik leiser
    case 6: {
        if (SVOL > 0) {
          SVOL--;
          dfPlayer.volume(SVOL);
          crm.webUpdate("SVOL", String(SVOL), true);
        }
      } break;
    //WLAN neu verbinden (nur wenn getrennt)
    case 7: {
        if (WiFi.status() != WL_CONNECTED && WiFi.RSSI() == 0) WiFi.reconnect();
      } break;
    //ESP32 neu starten
    case 8: {
        crm.webNotif("i", lng(L_NOTIF_REBOOT), 3);
        tgNotif(lng(L_NOTIF_REBOOT));
        mqttSend("s", lng(L_NOTIF_REBOOT));
        tgFlush(3000);
        crm.espReboot();
      } break;
    //Beleuchtung einschalten (russ. Original sagte fälschlich "aus")
    case 9: {
        lift_position = true;
        if (mLED) ledPowerAction(true);
      } break;
    //Beleuchtung ausschalten (russ. Original sagte fälschlich "ein")
    case 10: {
        ledPowerAction(false);
      } break;
    //Pumpe einschalten
    case 11: {
        pump_action(1);
      } break;
    //Pumpe ausschalten
    case 12: {
        pump_action(0);
      } break;
    default:
      break;
  }
}


void SendCode(long code, uint8_t port, uint8_t pacs) {
  Serial.println(String(code) + "\t" + String(port) + "\t" + String(pacs));
  RCSwitch rfSender = RCSwitch();
  rfSender.enableTransmit(port);
  //rfSender.setPulseLength(320);       //
  rfSender.setRepeatTransmit(pacs);     //Anzahl Wiederholungen pro Sendung
  rfSender.send(abs(code), 24);         //Code senden (24 Bit)
  Serial.println("[RF] Code \"" + String(abs(code)) + F("\" send."));
}
