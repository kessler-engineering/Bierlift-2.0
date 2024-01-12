
void interface() {
  xSemaphoreTake(wifi_mutex, portMAX_DELAY);

  //УПРАВЛЕНИЕ
  crm.page(lng(L_M_CONTROL));
  crm.output({OUTPUT_TABL, "STATUS", lng(L_STATUS), workState(), "fff"});
  if (VTIN || VTOUT) crm.output({OUTPUT_TABL, "TEMP", lng(L_TEMP), tempState(), "fff"});
  if (USEWS) crm.output({OUTPUT_TABL, "WATER", lng(L_WATER_LEVEL), waterState()});
  crm.output({OUTPUT_HR, "0", "20px"});
  if (OLD == false && HGSP == true) {
    crm.input({INPUT_BUTTON, "UP2", "&#xe804;&#xe804;", "10px 25px", "", "49"});
    crm.output({OUTPUT_HR, "0", "-25px 0"});
  }
  crm.input({INPUT_BUTTON, "UP1", "&#xe804;", "10px 48px 10px 52px", "", "45"});
  crm.output({OUTPUT_HR, "0", "-15px 0"});
  crm.input({INPUT_BUTTON, "STP", "&#xe806;", "3px 47px 3px 53px", "", "58"});
  crm.output({OUTPUT_HR, "0", "-15px 0"});
  crm.input({INPUT_BUTTON, "DN1", "&#xe805;", "10px 48px 10px 52px", "", "45"});
  if (OLD == false && HGSP == true) {
    crm.output({OUTPUT_HR, "0", "-25px 0"});
    crm.input({INPUT_BUTTON, "DN2", "&#xe805;&#xe805;", "10px 25px", "", "49"});
  }
  if (DP22 && DP22C || DP25 && DP25C || DP26 && DP26C) {
    crm.output({OUTPUT_HR, "1px", "40px 20% -20px"});
    if (DP22C) crm.input({INPUT_BUTTON, "DP22B", (digitalRead(DEV_PIN_22) == DP22S ? "&#xf204;  " : "&#xf205;  ") + crm.var("DP22N"), "", "r"});
    if (DP25C) crm.input({INPUT_BUTTON, "DP25B", (digitalRead(DEV_PIN_25) == DP25S ? "&#xf204;  " : "&#xf205;  ") + crm.var("DP25N"), "", "r"});
    if (DP26C) crm.input({INPUT_BUTTON, "DP26B", (digitalRead(DEV_PIN_26) == DP26S ? "&#xf204;  " : "&#xf205;  ") + crm.var("DP26N"), "", "r"});
  }


  //ПОДСВЕТКА
  crm.page(lng(L_M_LIGHT));
  crm.range({"LEDN", lng(L_LED_QUANTITY), 60, 1, MAX_LED_IN_STRIP, 1});
  crm.select({"mLED", lng(L_LED_EFFECTS), "0", {{lng(L_LED_E_OFF), "0"}, {lng(L_LED_E_STATIS_COLOR), "1"},
      {lng(L_LED_E_RAINBOW), "2"}, {lng(L_LED_E_SMOOTH_COLOR), "3"}, {lng(L_LED_E_SPARKLES), "4"},
      {lng(L_LED_E_GLITTER), "5"}, {lng(L_LED_E_FIRE_ORANGE), "6"}, {lng(L_LED_E_FIRE_YELLOW_RED), "7"},
      {lng(L_LED_E_FIRE_BLUE), "8"}, {lng(L_LED_E_FIRE_RED), "9"}, {lng(L_LED_E_FIRE_BLUE_GREEN), "10"},
      {lng(L_LED_E_SINELON), "11"}, {lng(L_LED_E_SHOW_SPARKLE), "12"}
    }
  });
  crm.input({INPUT_CHECKBOX, "INV", lng(L_REVERSE_DIRECT), "true"});
  crm.input({INPUT_COLOR, "COLOR", lng(L_LED_COLOR), "#00FF00"});
  crm.range({"SPEED", lng(L_EFFECT_SPEED), 95, 1, 100, 1});
  crm.range({"PARAM", lng(L_EFFECT_PARAMETR), 10, 1, 100, 1});
  crm.range({"BR", lng(L_BACKLIGHT_BRIGHT), 100, 1, 255, 1});
  crm.input({INPUT_BUTTON, "RBT", lng(L_REBOOT)});


  //ВОДА
  crm.page(lng(L_M_WATER));
  if (OLD == false) crm.input({INPUT_CHECKBOX, "USEPM", lng(L_USE_PUMP), "false", "1"});
  crm.input({INPUT_CHECKBOX, "USEWS", lng(L_USE_WATER_SENS), "false", "1"});
  if (OLD == false) {
    crm.input({INPUT_CHECKBOX, "PUMPM", lng(L_AUTO_MODE_PUMP), "false", "1"});
    crm.input({INPUT_CHECKBOX, "PUMPSENSINV", lng(L_INV_SENS_PUMP), "true"});
    if (USEPM) {
      if (PUMPM) crm.range({"DELAYPM", lng(L_PUMP_SHUDOWN_DELAY), 10, 1, 60, 1, lng(L_SEC)});
      else crm.input({INPUT_BUTTON, "PUMP", (pumpStatus ? lng(L_STOP) : lng(L_PUSK))});
    }
  }


  //ТЕМПЕРАТУРА
  crm.page(lng(L_M_TEMP));
  crm.input({INPUT_CHECKBOX, "VTIN", lng(L_EN_TEMP_MEAS_IN), "false", "1"});
  crm.input({INPUT_CHECKBOX, "VTOUT", lng(L_EN_TEMP_MEAS_OUT), "false", "1"});
  crm.input({INPUT_CHECKBOX, "TSWAP", lng(L_SWAP_TEMP_SENS_VOL), "false", "1"});
  if (VTIN && VTOUT) {
    crm.input({INPUT_CHECKBOX, "NCOOL", lng(L_NIGTH_COOLING), "false", "1"});
    if (NCOOL) {
      crm.output({OUTPUT_TABL, "TM", lng(L_TIME), curTime(), "fff"});
      if (WTHR) crm.output({OUTPUT_TABL, "WR", lng(L_WEATHER), weatherString(), "fff"});
      crm.input({INPUT_NUMBER, "TZ", lng(L_TIME_ZONE), "120"});
      crm.input({INPUT_TIME, "TOPN", lng(L_TIME_OPEN)});
      crm.input({INPUT_TIME, "TCSE", lng(L_TIME_CLOSE)});
      crm.range({"TGIST", lng(L_TEMP_GISTER)+" Tin - Tout", 5, 0, 20, 1, " °C"});
      crm.range({"TMOVE", lng(L_ASCENT_TIME), 3000, 300, 10000, 50, lng(L_MSEC)});
      crm.input({INPUT_CHECKBOX, "WTHR", lng(L_CONSIDER_THE_WEATHER), "false", "1"});
      if (WTHR) {
        crm.input({INPUT_TEXT, "WID", lng(L_COORDINATS)+" (lat, lon)", ""});
        crm.range({"WSPD", lng(L_WIND_SPEED), 20, 1, 30, 0.1, lng(L_MS)});
      }
    }
  }

  //ЗВУК
  crm.page(lng(L_M_SOUND));
  crm.input({INPUT_CHECKBOX, "SUSE", lng(L_USE_SOUND), "false", "1"});
  crm.range({"SVOL", lng(L_SOUND_VOL), 10, 1, 30, 1});
  crm.select({"EQ", lng(L_EQUALIZER), "0", {{lng(L_EQ_NORMAL), "0"},
      {lng(L_EQ_POP), "1"}, {lng(L_EQ_ROCK), "2"}, {lng(L_EQ_JAZZ), "3"},
      {lng(L_EQ_CLASSIC), "4"}, {lng(L_EQ_BASS), "5"}
    }
  });
  crm.input({INPUT_CHECKBOX, "SRND", lng(L_RANDOM_SOUND), "false", "1"});
  crm.input({INPUT_TEXT, "SUP", lng(L_LIFTING_SOUND), "1"});
  crm.input({INPUT_TEXT, "SDOWN", lng(L_DESCENT_SOUND), "1"});
  if (SUSE) {
    crm.output({OUTPUT_HR, "1px", "35px 10%"});
    String list = "";
    for (int i = 1; i <= fileCounts; i++) {
      if (list != "") list += ",";
      list += "[\"" + lng(L_M_SOUND) + " " + String(i) + "\"," + String(i) + "]";
    }
    crm.select({"SDEMO", lng(L_SOUND_LIST), "1"}, list);
    crm.output({OUTPUT_HR, "0", "-20px"});
    crm.input({INPUT_BUTTON, "PSD", "&#xe816;", "", "r", "20", "1"});
    crm.input({INPUT_BUTTON, "PD", "&#xe811;", "", "r", "20"});
    crm.input({INPUT_BUTTON, "SD", "&#xe812;", "", "r", "20"});
    crm.input({INPUT_BUTTON, "NSD", "&#xe815;", "", "r", "20", "1"});
    list = "";
  }


  //BLYNK
  crm.page(lng(L_M_BLYNK));
  if (BUSE) {
    crm.output({OUTPUT_TABL, "BST", lng(L_BLYNK_STATUS), "n/a"});
    crm.output({OUTPUT_HR, "1px", "20px"});
  }
  crm.input({INPUT_CHECKBOX, "BUSE", lng(L_USE_BLYNK), "false", "1"});
  crm.input({INPUT_TEXT, "BAPI", lng(L_BLYNK_API), ""});
#ifdef BLYNK_TEMPLATE_ID
  crm.input({INPUT_TEXT, "BTID", "BLYNK TEMPLATE ID", ""});
#endif
  crm.input({INPUT_BUTTON, "RBT", lng(L_REBOOT)});


  //RF
  crm.page(lng(L_WIRE_REMOTE));
  crm.input({INPUT_CHECKBOX, "USERF", lng(L_USE_CODE), "false", "1"});
  //if (USERF) {
  String list = "";
  crm.output({OUTPUT_TABL, "LCODE", lng(L_LAST_CODE), String(lastReceivedCode)});
  for (int i = 0; i < 13; i++) {
    if (list != "") list += ",";
    list += "[\"" + lng(113 + i) + "\"," + String(i) + "]";
  }
  for (int i = 1; i < 5; i++) {
    if (i > 1) crm.output({OUTPUT_HR, "1px", "46px 20% 50px"});
    crm.input({INPUT_TEXT, ("CODE" + String(i)), lng(24 + i)});
    crm.select({("CODE" + String(i) + "A"), lng(L_CODEA), "0"}, list);
  }
  list = "";
  //}


  //Telegram
  crm.page(lng(L_M_TELEGA));
  crm.input({INPUT_CHECKBOX, "TGUSE", lng(L_USE_TELEGA), "false"});
  crm.input({INPUT_CHECKBOX, "TGNOTIF", lng(L_TELEGA_NOTIF), "false"});
  crm.output({OUTPUT_HR, "1px", "36px 20% 50px"});
  crm.input({INPUT_TEXT, "TGBOT", lng(L_TELEGA_BOT), ""});
  crm.input({INPUT_TEXT, "TGID", lng(L_TELEGA_ID), ""});
  crm.input({INPUT_BUTTON, "RBT", lng(L_REBOOT)});


  //MQTT
  crm.page("MQTT");
  crm.input({INPUT_CHECKBOX, "MQUSE", "MQTT", "false"});
  crm.input({INPUT_TEXT, "MQSRV", lng(L_MQTT_SERVER), ""});
  crm.input({INPUT_NUMBER, "MQPRT", lng(L_MQTT_PORT), "1883"});
  crm.input({INPUT_TEXT, "MQLG", lng(L_LOGIN), ""});
  crm.input({INPUT_PASSWORD, "MQPW", lng(L_PASS), ""});
  crm.input({INPUT_TEXT, "MQTP", lng(L_MQTT_PREFIX), ""});


  //WI-FI
  crm.page(lng(L_M_WIFI));
  crm.wifiForm(WIFI_AP, F("BierliftKE"));
  crm.input({INPUT_BUTTON, "RBT", lng(L_REBOOT)});


  //ДВИЖЕНИЕ
  crm.page(lng(L_M_MOVE));
  crm.range({"UTIME", lng(L_ASCENT_TIME), 20, 1, 60, 1, lng(L_SEC)});
  crm.range({"DTIME", lng(L_DESCENT_TIME), 20, 1, 60, 1, lng(L_SEC)});
  if (OLD == false) {
    crm.input({INPUT_CHECKBOX, "MOP", lng(L_USE_MOVE_SENSOR), "false"});
    crm.input({INPUT_CHECKBOX, "HGSP", lng(L_EN_HIGH_SPEED_MODE), "false", "1"});
    crm.input({INPUT_CHECKBOX, "SINV", lng(L_INV_SPEED_CONTROL_SIG), "false"});
  }


  //РАЗНОЕ
  crm.page(lng(L_M_OTHER));
  crm.input({INPUT_CHECKBOX, "OLD", lng(L_ADAPT_OLD), "false", "1"});
  crm.input({INPUT_CHECKBOX, "CHP", lng(L_CHILD_PROT), "false"});
  crm.input({INPUT_CHECKBOX, "LFLASH", lng(L_LFLASH), "true"});
  crm.input({INPUT_CHECKBOX, "CHUDT", lng(L_CHUDT), "true"});
  crm.input({INPUT_BUTTON, "BTCUDT", lng(L_UDT_SEARCH)});   //Кнопка для ручного проверки обновления
  crm.output({OUTPUT_HR, "1px", "25px 0 0"});
  crm.input({INPUT_BUTTON, "RBT", lng(L_REBOOT)});


  //РАЗРАБОТКА
  crm.page(lng(L_M_DEVELOP));
  crm.input({INPUT_CHECKBOX, "DP22", lng(L_D_PIN) + "22", "false", "1"});
  if (DP22) {
    crm.input({INPUT_CHECKBOX, "DP22S", lng(L_D_FERST_SOST_PIN) + " (LOW | HIGH)", "false"});
    crm.input({INPUT_CHECKBOX, "DP22U", lng(L_D_ACTION_IF) + " <z>&#xe804;</z>", "false"});
    crm.input({INPUT_CHECKBOX, "DP22NO", lng(L_D_DONT_TURN_OFF_AT_THE_TOP), "false"});
    crm.input({INPUT_CHECKBOX, "DP22D", lng(L_D_ACTION_IF) + " <z>&#xe805;</z>", "false"});
    crm.input({INPUT_CHECKBOX, "DP22C", lng(L_D_VIEW_BT_ON_MAIM), "false", "1"});
    if (DP22C) crm.input({INPUT_TEXT, "DP22N", lng(L_D_BT_NAME), "GPIO 22"});
  }
  if (DP22 || DP25) crm.output({OUTPUT_HR, "1px", "30px 10% 30px"});
  crm.input({INPUT_CHECKBOX, "DP25", lng(L_D_PIN) + "25", "false", "1"});
  if (DP25) {
    crm.input({INPUT_CHECKBOX, "DP25S", lng(L_D_FERST_SOST_PIN) + " (LOW | HIGH)", "false"});
    crm.input({INPUT_CHECKBOX, "DP25U", lng(L_D_ACTION_IF) + " <z>&#xe804;</z>", "false"});
    crm.input({INPUT_CHECKBOX, "DP25NO", lng(L_D_DONT_TURN_OFF_AT_THE_TOP), "false"});
    crm.input({INPUT_CHECKBOX, "DP25D", lng(L_D_ACTION_IF) + " <z>&#xe805;</z>", "false"});
    crm.input({INPUT_CHECKBOX, "DP25C", lng(L_D_VIEW_BT_ON_MAIM), "false", "1"});
    if (DP25C) crm.input({INPUT_TEXT, "DP25N", lng(L_D_BT_NAME), "GPIO 25"});
  }
  if (DP26 || DP25) crm.output({OUTPUT_HR, "1px", "30px 10% 30px"});
  crm.input({INPUT_CHECKBOX, "DP26", lng(L_D_PIN) + "26", "false", "1"});
  if (DP26) {
    crm.input({INPUT_CHECKBOX, "DP26S", lng(L_D_FERST_SOST_PIN) + " (LOW | HIGH)", "false"});
    crm.input({INPUT_CHECKBOX, "DP26U", lng(L_D_ACTION_IF) + " <z>&#xe804;</z>", "false"});
    crm.input({INPUT_CHECKBOX, "DP26NO", lng(L_D_DONT_TURN_OFF_AT_THE_TOP), "false"});
    crm.input({INPUT_CHECKBOX, "DP26D", lng(L_D_ACTION_IF) + " <z>&#xe805;</z>", "false"});
    crm.input({INPUT_CHECKBOX, "DP26C", lng(L_D_VIEW_BT_ON_MAIM), "false", "1"});
    if (DP26C) crm.input({INPUT_TEXT, "DP26N", lng(L_D_BT_NAME), "GPIO 26"});
  }

  //Уведомление о новой версии
  if (CHUDT && newVerSt() == 2 && !crm.webConnCountStatus()) {
    udtNotif.once_ms(2000, udtNotifSend);
  }

  xSemaphoreGive(wifi_mutex);
}


String workState() {
  if (releyStatus) {
    bool U = digitalRead(UP_PIN),
         D = digitalRead(DOWN_PIN);
    if (U && D) return lng(L_SIM_RELAY_ACTIV);
    else if (releyCommandStatus) {
      return lng(L_JUST_A_MOMENT);
    } else {
      if (releyDirection) return String(motorSpeed ? lng(L_RISE_2X_UP) : lng(L_RISE_UP)) + (String((UTIME - (millis() - releyOffTimer)) / 1000) + lng(L_S));
      else return String(motorSpeed ? lng(L_RISE_2X_DOWN) : lng(L_RISE_DOWN)) + (String((DTIME - (millis() - releyOffTimer)) / 1000) + lng(L_S));
    }
  } else {
    if (USEPM && pumpStatus) return lng(L_PUMP_IS_WORK);
    else return lng(L_WAIT_ORDER);
  }
}
