
bool tgAPI(TBMessage m) {

  String t = m.text;
  String id = crm.var("TGID");
  if (id != "" && (int64_t)id.toInt() != m.chatId) {
    tgBot.sendMessage(m, String(m.sender.username) + " (ID " + String((uint32_t)m.chatId) + lng(L_TELEGA_s1));
    tgNotif(lng(L_TELEGA_s2) + String(m.sender.username) + " (ID " + String((uint32_t)m.chatId) + "):  " + m.text);
    return false;
  }

  if (t.equalsIgnoreCase("↑↑")) {
    tgBot.sendMessage(m, "OK");
    releyAction(2);
  }

  if (t.equalsIgnoreCase("↑")) {
    tgBot.sendMessage(m, "OK");
    releyAction(1);
  }

  if (t.equalsIgnoreCase("stop")) {
    tgBot.sendMessage(m, "OK");
    releyAction(0);
  }

  if (t.equalsIgnoreCase("↓")) {
    tgBot.sendMessage(m, "OK");
    releyAction(-1);
  }

  if (t.equalsIgnoreCase("↓↓")) {
    tgBot.sendMessage(m, "OK");
    releyAction(-2);
  }

  if (t.equalsIgnoreCase("status")) {
    tgBot.sendMessage(m, workState());
  }

  if (t.equalsIgnoreCase("temp")) {
    String s = tempState();
    s.replace("\\n", "\n");
    Serial.println(s);
    tgBot.sendMessage(m, s);
  }

  return true;
}


void telegram(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  ReplyKeyboard keyBoard;
  bool init = false;
  for (;;) {
    if (WiFi.status() == WL_CONNECTED) {
      if (init) {
        TBMessage msg;
        if (tgBot.getNewMessage(msg)) {
          Serial.println("Message from @" + String(msg.sender.username) + " (ID " + String((uint32_t)msg.chatId) + "):\t" + msg.text);
          if (msg.text.equalsIgnoreCase("/start"))
            tgBot.sendMessage(msg, "I'm ready 🚀", keyBoard);
          if (msg.text.equalsIgnoreCase("hide keyboard"))
            tgBot.removeReplyKeyboard(msg, "Keyboard hide");
          tgAPI(msg);
        }
      } else {
        tgBot.setUpdateTime(2000);
        String t = crm.var("TGBOT");
        char *cstr = new char[t.length() + 1];
        strcpy(cstr, t.c_str());
        tgBot.setTelegramToken(cstr);
        Serial.println("\n[TG] Telegram connection... ");
        init = tgBot.begin() ? Serial.println("DONE") : Serial.println("FAILED");
        if (init) tgNotif(lng(L_TELEGA_ONLINE));

        keyBoard.addButton("↑↑");
        keyBoard.addButton("↑");
        keyBoard.addRow();
        keyBoard.addButton("status");
        keyBoard.addButton("stop");
        keyBoard.addButton("temp");
        keyBoard.addRow();
        keyBoard.addButton("↓↓");
        keyBoard.addButton("↓");
        /*keyBoard.addRow();
        keyBoard.addButton("hide keyboard");*/
        keyBoard.enableResize();
      }
    }
    vTaskDelay(100);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))
      Serial.println("[HEAP] Telegram: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


void tgNotif(const String &m) {
  if (TGUSE && TGNOTIF && WiFi.status() == WL_CONNECTED) {
    String id = crm.var("TGID");
    if (id != "")
      tgBot.sendTo((int64_t)id.toInt(), m);
  }
}

// msg.sender.languageCode   язык
// msg.text                  текст
// msg.sender.username       имя пользователя
// msg.chatId                id пользователя
