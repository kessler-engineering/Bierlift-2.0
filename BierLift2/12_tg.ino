
//Telegram-Menü (Tasten der Bot-Tastatur)
const String &tgMenu(F("↑↑\t↑\nstatus\tstop\ttemp\n↓↓\t↓"));


void tgMessage(FB_msg &msg) {
  {
    String m(F("[TG] "));
    m += msg.username;
    m += F(" (");
    m += msg.userID;
    m += F("): ");
    m += msg.text;
    Serial.println(m);
  }

  if (msg.userID == crm.var("TGID")) {
    const String &t(msg.text);
    const String &OK(F("OK"));

    if (t.equalsIgnoreCase("↑↑")) {
      tg.sendMessage(OK);
      releyAction(2);
    }

    else if (t.equalsIgnoreCase("↑")) {
      tg.sendMessage(OK);
      releyAction(1);
    }

    else if (t.equalsIgnoreCase("stop")) {
      tg.sendMessage(OK);
      releyAction(0);
    }

    else if (t.equalsIgnoreCase("↓")) {
      tg.sendMessage(OK);
      releyAction(-1);
    }

    else if (t.equalsIgnoreCase("↓↓")) {
      tg.sendMessage(OK);
      releyAction(-2);
    }

    else if (t.equalsIgnoreCase("status")) {
      tg.sendMessage(workState());
    }

    else if (t.equalsIgnoreCase("temp")) {
      String m(tempState());
      m.replace("\\n", "\n");
      Serial.println(m);
      tg.sendMessage(m);
    }

    else {
      tg.showMenuText(lng(L_TG_INVALIDE_COMMAND), tgMenu);
    }

  } else {
    String m(lng(L_TELEGA_s2));
    m += msg.username;
    m += F(" (ID ");
    m += msg.userID;
    m += F("):  ");
    m += msg.text;
    tgNotif(m);
  }
}


void telegram(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  bool init = false;
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      if (init) {
        tg.tick();
        // Gepufferte Meldungen senden (tgNotif() aus anderen Tasks/loop()).
        String *p = NULL;
        while (tgQueue != NULL && xQueueReceive(tgQueue, &p, 0) == pdTRUE) {
          if (p != NULL) {
            tg.showMenuText(*p, tgMenu);
            delete p;
          }
        }
      } else {
        tg.setToken(crm.var("TGBOT"));
        tg.setChatID(crm.var("TGID"));
        tg.setPeriod(2000);
        tg.attach(tgMessage);
        tg.showMenuText(String(("🍺 ")) + lng(L_TELEGA_ONLINE), tgMenu);
        init = 1;
      }
    }
    vTaskDelay(500);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))
      Serial.println("[HEAP] Telegram: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


// Stellt die Meldung nur in die Queue – gesendet wird im Telegram-Task.
// Früher lief hier eine blockierende HTTPS-Anfrage, auch aus loop() bei jedem Fahrbefehl.
void tgNotif(const String & m) {
  if (TGUSE && TGNOTIF && tgQueue != NULL && WiFi.status() == WL_CONNECTED) {
    String id = crm.var("TGID");
    if (id != "") {
      String *p = new String(m);
      if (xQueueSend(tgQueue, &p, 0) != pdTRUE) delete p;   // Queue voll: Meldung verwerfen
    }
  }
}


// Vor Neustart/Reset: kurz warten, bis der Telegram-Task die Queue geleert hat.
void tgFlush(uint32_t timeout_ms) {
  if (!TGUSE || telegram_task == NULL || tgQueue == NULL) return;
  uint32_t t = millis();
  while (uxQueueMessagesWaiting(tgQueue) > 0 && millis() - t < timeout_ms) vTaskDelay(50);
  vTaskDelay(300);   // letzte Anfrage abschließen lassen
}
