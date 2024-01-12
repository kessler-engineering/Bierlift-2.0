
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
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //TEST
      Serial.println("[HEAP] RF: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


void rfAction(int a) {
  switch (a) {
    //ехать вверх/вниз/стоп (поочереди)
    case 1: {
        if (releyStatus) {
          releyAction(0);
          releyDirection = !releyDirection;
        } else releyDirection ? releyAction(1) : releyAction(-1);
      } break;
    //ехать вверх
    case 2: {
        releyAction (1);
      } break;
    //ехать вниз
    case 3: {
        releyAction (-1);
      } break;
    //Стоп
    case 4: {
        releyAction (0);
      } break;
    //музыка громче
    case 5: {
        if (SVOL < 31) {
          SVOL++;
          dfPlayer.volume(SVOL);
          crm.webUpdate("SVOL", String(SVOL), true);
        }
      } break;
    //музыка тише
    case 6: {
        if (SVOL > 0) {
          SVOL--;
          dfPlayer.volume(SVOL);
          crm.webUpdate("SVOL", String(SVOL), true);
        }
      } break;
    //рестарт вайфай
    case 7: {
        if (WiFi.status() != WL_CONNECTED && WiFi.RSSI() == 0) WiFi.reconnect();
      } break;
    //ребут ЕСП32
    case 8: {
        crm.webNotif("i", lng(L_NOTIF_REBOOT), 3);
        tgNotif(lng(L_NOTIF_REBOOT));
        mqttSend("s", lng(L_NOTIF_REBOOT));
        crm.espReboot();
      } break;
    //выключить подсветку
    case 9: {
        lift_position = true;
        if (mLED) ledPowerAction(true);
      } break;
    //включить подсветку
    case 10: {
        ledPowerAction(false);
      } break;
    //включить помпу
    case 11: {
        pump_action(1);
      } break;
    //выключить помпу
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
  rfSender.setRepeatTransmit(pacs);     //Количество посылок в пакете
  rfSender.send(abs(code), 24);         //Отправка кода
  Serial.println("[RF] Code \"" + String(abs(code)) + F("\" send."));
}
