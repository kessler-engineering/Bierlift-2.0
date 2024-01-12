
void ds18_read(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  OneWire oneWireDS(DS18_PIN);
  DallasTemperature ds18(&oneWireDS);
  bool ds18b20_init = true;
  float TinOld = 0, ToutOld = 0;
  for (;;) {
    xSemaphoreTake(wifi_mutex, portMAX_DELAY);
    if (ds18b20_init) {
      ds18b20_init = false;
      Serial.println("[SENSOR] Initialization sensor DS18B20...");
      ds18.begin();
      Serial.printf("[SENSOR] Found %d devices DS18B20.\n", ds18.getDeviceCount());
      if (ds18.getResolution() != 12) ds18.setResolution(12);
      ds18.setWaitForConversion(true);    //Ожидать вычисления
    }

    ds18.requestTemperatures();
    Tin =  median_t0(ds18.getTempCByIndex(TSWAP ? 0 : 1));
    Tout =  median_t1(ds18.getTempCByIndex(TSWAP ? 1 : 0));
    delay(100);

    if (BUSE && blynkConnSt) {
      Blynk.virtualWrite(V1, Tin);
      Blynk.virtualWrite(V4, Tout);
    }

    if (TinOld != Tin) {
      TinOld = Tin;
      mqttSend("t/in", String(Tin, 1));
    }
    if (ToutOld != Tout) {
      ToutOld = Tout;
      mqttSend("t/out", String(Tout, 1));
    }

    xSemaphoreGive(wifi_mutex);

    crm.webUpdate("TEMP", tempState());

    //Serial.printf("Temperature Tin %.1f, Tout %.1f\n", Tin, Tout);
    vTaskDelay(5000);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //TEST
      Serial.println("[HEAP] Temp: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


String tempState() {
  String t = String();
  if (VTIN) t = lng(L_T_IN) + (Tin != -127 ? (String(Tin , 1) + String(" °C")) : lng(L_SENS_ERR));
  if (VTOUT) {
    if (VTIN) t += F("\\n");
    t += lng(L_T_OUT) + (Tout != -127 ? (String(Tout , 1) + String(" °C")) : lng(L_SENS_ERR));
  }
  return t != "" ? t : "Sensors are not used.";
}


void waterSensRead(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  bool wOld = waterVol;
  for (;;) {
    waterVol = PUMPSENSINV ? !digitalRead(WATER_SENS_PIN) : digitalRead(WATER_SENS_PIN);
    if (wOld != waterVol) {
      wOld = waterVol;
      crm.webUpdate("WATER", waterState());
      mqttSend("w", String(waterVol));
      tgNotif(lng(L_WATER_LVL) + String(waterVol));
    }
    vTaskDelay(1000);
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //TEST
      Serial.println("[HEAP] Water: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


String waterState() {
  return (waterVol ?  "<span style='color:#f00;'>" + lng(L_HIGH) +
          "</span>" : "<span style='color:#0f0;'>" + lng(L_LOW) + "</span>");
}


bool optosensor() {
  if (OLD == false && MOP && optoStatus && millis() - optoTimer >= WAIT_FOR_POWER_SENSOR) {
    if (digitalRead(OPTO_SENSOR_PIN) == HIGH) return false;
    else return true;
  }
  return false;
}


float median_t0(float newVal) {
  static float buf[3];
  static byte count = 0;
  buf[count] = newVal;
  if (++count >= 3) count = 0;
  float a = buf[0], b = buf[1], c = buf[2], middle;
  if ((a <= b) && (a <= c)) middle = (b <= c) ? b : c;
  else {
    if ((b <= a) && (b <= c)) middle = (a <= c) ? a : c;
    else middle = (a <= b) ? a : b;
  }
  return middle;
}


float median_t1(float newVal) {
  static float buf[3];
  static byte count = 0;
  buf[count] = newVal;
  if (++count >= 3) count = 0;
  float a = buf[0], b = buf[1], c = buf[2], middle;
  if ((a <= b) && (a <= c)) middle = (b <= c) ? b : c;
  else {
    if ((b <= a) && (b <= c)) middle = (a <= c) ? a : c;
    else middle = (a <= b) ? a : b;
  }
  return middle;
}
