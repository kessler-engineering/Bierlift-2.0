
// Löscht einen Task, der den wifi_mutex benutzt (Blynk, DS18B20).
// Erst den Mutex holen: würde der Task gelöscht, während er ihn hält,
// bliebe der Mutex für immer belegt (API, Temperatur, Blynk hängen).
void deleteMutexTask(TaskHandle_t &h) {
  if (h == NULL) return;
  xSemaphoreTake(wifi_mutex, portMAX_DELAY);
  vTaskDelete(h);
  h = NULL;
  xSemaphoreGive(wifi_mutex);
}


void update() {
  byte mLEDold = mLED, C1old = C1, C2old = C2, C3old = C3, SVOLold = SVOL,
       EQold = EQ, SINVold = SINV;
  bool OLDold = OLD, USEPMold = USEPM, MQUSEold = MQUSE;

  String lang = crm.var("_L");
  LNG = 0;
  if (lang == "de") LNG = 1;
  else if (lang == "ru") LNG = 2;

  UTIME = 1000UL * crm.var("UTIME").toInt();
  DTIME = 1000UL * crm.var("DTIME").toInt();
  HGSP = crm.var("HGSP") == "true";
  OLD = crm.var("OLD") == "true";
  VTIN = crm.var("VTIN") == "true";
  VTOUT = crm.var("VTOUT") == "true";
  TSWAP = crm.var("TSWAP") == "true";
  PUMPM = crm.var("PUMPM") == "true";
  PUMPSENSINV = crm.var("PUMPSENSINV") == "true";
  USEPM = crm.var("USEPM") == "true";
  SUSE = crm.var("SUSE") == "true";
  BUSE = crm.var("BUSE") == "true";
  MOP = crm.var("MOP") == "true";
  SINV = crm.var("SINV") == "true";
  USERF = crm.var("USERF") == "true";
  CHP = crm.var("CHP") == "true";
  USEWS = crm.var("USEWS") == "true";
  SRND = crm.var("SRND") == "true";
  TGUSE = crm.var("TGUSE") == "true";
  TGNOTIF = crm.var("TGNOTIF") == "true";
  LFLASH = crm.var("LFLASH") == "true";
  MQUSE = crm.var("MQUSE") == "true";
  CHUDT = crm.var("CHUDT") == "true";
  DELAYPM = 1000UL * crm.var("DELAYPM").toInt();
  dev_update();
  CODE1 = crm.var("CODE1").toInt();
  CODE2 = crm.var("CODE2").toInt();
  CODE3 = crm.var("CODE3").toInt();
  CODE4 = crm.var("CODE4").toInt();
  mLED = crm.var("mLED").toInt();
  INV = crm.var("INV") == "true";
  SPEED = 101 - crm.var("SPEED").toInt();
  PARAM = crm.var("PARAM").toInt();
  SVOL = crm.var("SVOL").toInt();
  EQ = crm.var("EQ").toInt();
  String COLOR = crm.var("COLOR");
  C1 = StrToDec(COLOR.substring(1, 3));
  C2 = StrToDec(COLOR.substring(3, 5));
  C3 = StrToDec(COLOR.substring(5, 7));
  if (USEPMold != USEPM && !USEPM) {
    pump_action(0);
    if (PUMPM && task_pump_handle != NULL) {
      vTaskDelete(task_pump_handle);
      task_pump_handle = NULL;
      crm.var("PUMPM", "false", false);
      PUMPM = false;
      crm.webUpdate("PUMPM", "false");
    }
  }
  if (OLDold != OLD) {
    if (OLD) {
      ledcSetup(0, PWM_FREQ, 8);
      ledcAttachPin(PUMP_PIN, 0);
      pinMode(PUMP_PIN, OUTPUT);
      ledcWrite(0, 0);
    } else {
      ledcDetachPin(PUMP_PIN);
      pinMode(PUMP_PIN, OUTPUT);
      digitalWrite(PUMP_PIN, LOW);
    }
  }
  if (mLED) {
    mainBR = crm.var("BR").toInt();
    if (mLED > 5) FireModeChange(mLED);
    if (!brightnessStatus && lift_position) brightnessStatus = true;
  }
  if (lift_position) {
    if (mLED == 1 && (C1old != C1 || C2old != C2 || C3old != C3)) setAll(CRGB(C1, C2, C3));
    if (mLEDold != mLED) mLED == 0 ? ledPowerAction(false) : ledPowerAction(true);
  }
  df_init();
  if (SUSE && dfInit && SVOLold != SVOL) dfPlayer.volume(SVOL);
  if (SUSE && dfInit && EQold != EQ) dfPlayer.EQ(EQ);
  if (!SUSE && dfInit) dfInit = false;
  if (SINVold != SINV) digitalWrite(SEED_PIN, !digitalRead(SEED_PIN));

  if (USEWS && task_waterSensRead == NULL) xTaskCreate(waterSensRead, "waterSensRead", 3000+5000, NULL, 1, &task_waterSensRead);
  else if (!USEWS && task_waterSensRead != NULL) {
    vTaskDelete(task_waterSensRead);
    task_waterSensRead = NULL;
    if (task_pump_handle != NULL) {
      vTaskDelete(task_pump_handle);
      task_pump_handle = NULL;
      crm.var("PUMPM", "false", false);
      PUMPM = false;
      crm.webUpdate("PUMPM", "false");
    }
  }
  if (USEPM && PUMPM && !OLD && task_pump_handle == NULL) {
    xTaskCreate(task_pump, "task_pump", 5000, NULL, 1, &task_pump_handle);
    if (task_waterSensRead == NULL) {
      USEWS = true;
      crm.webUpdate("USEWS", "true");
      crm.var("USEWS", "true", false);
      xTaskCreate(waterSensRead, "waterSensRead", 5000, NULL, 1, &task_waterSensRead);
    }
  } else if ((!USEPM || !PUMPM || OLD) && task_pump_handle != NULL) {
    vTaskDelete(task_pump_handle);
    task_pump_handle = NULL;
  }
  if ((VTIN || VTOUT) && ds18_read_handle == NULL) xTaskCreatePinnedToCore(ds18_read, "ds18_read", 8000, NULL, 1, &ds18_read_handle, 0);
  else if (!VTIN && !VTOUT && ds18_read_handle != NULL) {
    deleteMutexTask(ds18_read_handle);
  }
  if (BUSE && blynk_start_task == NULL) xTaskCreatePinnedToCore(blynk_start, "blynk_start", 7000, NULL, 1, &blynk_start_task, 0);
  else if (!BUSE && blynk_start_task != NULL) {
    deleteMutexTask(blynk_start_task);
  }
  if (TGUSE && telegram_task == NULL) xTaskCreatePinnedToCore(telegram, "telegram", 10000, NULL, 1, &telegram_task, 0);
  else if (!TGUSE && telegram_task != NULL) {
    vTaskDelete(telegram_task);
    telegram_task = NULL;
  }
  if (LFLASH && ledFlash_task == NULL) xTaskCreatePinnedToCore(ledFlash, "ledFlash", 5000, NULL, 1, &ledFlash_task, 1);
  else if (!LFLASH && ledFlash_task != NULL) {
    vTaskDelete(ledFlash_task);
    ledFlash_task = NULL;
  }
  if (USERF && rfControl_task == NULL) xTaskCreatePinnedToCore(rfControl, "rfControl", 7000, NULL, 1, &rfControl_task, 0);
  else if (!USERF && rfControl_task != NULL) {
    vTaskDelete(rfControl_task);
    rfControl_task = NULL;
  }

  if (CHUDT && metric_task == NULL) xTaskCreatePinnedToCore(metricRun, "UpdateChecker", 8000, NULL, 1, &metric_task, tskNO_AFFINITY);
  else if (!CHUDT && metric_task != NULL) {
    vTaskDelete(metric_task);
    metric_task = NULL;
  }

  if (MQUSEold != MQUSE) {
    MQUSEold = MQUSE;
    if (MQUSE) mqttInit(1);
    else mqttInit(0);
  }

  //Uhrzeit, Datum, Night Cooling
  TOPN = timeStrToMin(crm.var("TOPN"));
  TCSE = timeStrToMin(crm.var("TCSE"));
  TGIST = crm.var("TGIST").toInt();
  TMOVE = crm.var("TMOVE").toInt();
  WTHR = crm.var("WTHR") == "true";
  // Wetterabfrage läuft in eigenem Task (früher blockierend in loop()).
  // Der Task wird nie gelöscht (sonst Abbruch mitten in einer HTTP-Anfrage);
  // weatherRun() tut nichts, solange WTHR/VTIN/VTOUT aus sind.
  if (WTHR && weather_task == NULL) xTaskCreatePinnedToCore(weatherTask, "weather", 10000, NULL, 1, &weather_task, 0);
  WSPD = crm.var("WSPD").toFloat();
  bool _NCOOL = NCOOL;
  NCOOL = crm.var("NCOOL") == "true";
  if (_NCOOL != NCOOL) {
    if (NCOOL) timeInit();
    else {
      timeTicTicker.detach();
      timeSyncTicker.detach();
    }
  }
  int _TZ = TZ;
  bool _DSTEU = DSTEU;
  TZ = crm.var("TZ").toInt();
  DSTEU = crm.var("DSTEU") == "true";
  if (_TZ != TZ || _DSTEU != DSTEU) applyTimeZone();

}
