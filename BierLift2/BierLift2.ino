#include "00_sett.h"
#include "CRMui3.h"
#include "CRMbutton.h"
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <Ticker.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <FastLED.h>
#include <DFRobotDFPlayerMini.h>
#include <BlynkSimpleEsp32.h>
#include <RCSwitch.h>
#include <cstdio>
#include <AsyncMqttClient.h>



CRMui3 crm;
Ticker mqttTick;
Ticker udtNotif;
Ticker timeTicTicker;
Ticker timeSyncTicker;
Ticker nightCollTicker;
CRMbutton<BUTTON_PIN> bt;
CRGB leds[MAX_LED_IN_STRIP];
HardwareSerial dfSerial(1);
DFRobotDFPlayerMini dfPlayer;
AsyncMqttClient mqttClient;

//Telegram-Bot
#include <FastBot.h>
FastBot tg("");



void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName) {
  std::printf("\n[ERROR] Stack Overflow %s: %s\n", __func__, pcTaskName);
}

void setup() {
  wifi_mutex = xSemaphoreCreateMutex();
  loopTask_h = xTaskGetCurrentTaskHandle();         // setup() und loop() laufen im selben Task
  liftCmdQueue = xQueueCreate(8, sizeof(int));
  tgQueue = xQueueCreate(10, sizeof(String *));

  pinMode(OPTO_SENSOR_PIN, INPUT);
  pinMode(WATER_SENS_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);
  digitalWrite(PUMP_PIN, LOW);
  pinMode(UP_PIN, OUTPUT);
  digitalWrite(UP_PIN, LOW);
  pinMode(DOWN_PIN, OUTPUT);
  digitalWrite(DOWN_PIN, LOW);
  pinMode(SEED_PIN, OUTPUT);
  digitalWrite(SEED_PIN, LOW);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  lastVER = verToNum(CRM_VER);

#ifdef USE_ARDUINO_OTA
  crm.useArduinoOta();
#endif
  crm.begin(BL_NAME, interface, update, api, 115200);
  //crm.setWebAuth("admin", "admin");
  //crm.setApiKey("a123");

  dev_init();

  // Nur im Browser begrenzt – hier hart begrenzen, sonst Überlauf von leds[MAX_LED_IN_STRIP]
  LEDN = constrain(crm.var("LEDN").toInt(), 1, MAX_LED_IN_STRIP);
  FastLED.addLeds<WS2812, LED_DIN_PIN, GRB>(leds, LEDN).setCorrection(TypicalLEDStrip);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MAX_POWER_SUPPLY_CURRENT);
  FastLED.clear();
  FastLED.show();

  if (SAVE_LIFT_STATUS) lift_position = crm.var("LPOS") == "1";
}


void loop() {
  crm.run();
  button_hw();
  button_web();
  liftCmdQueueHandle();
  reley_handle();
  led_handle();
  if (releyStatus && millis() - stateTimer >= 1000) {
    stateTimer = millis();
    crm.webUpdate("STATUS", workState());
  }

#ifdef DBGRUN
  dbgRun();
#endif
}


uint16_t StrToDec(String S) {
  uint16_t d = 0;
  int n = 0;
  for (int i = 0; i < S.length(); i++) {
    n = int(S.charAt(i));
    if (n >= 48 && n <= 57) n = map(n, 48, 57, 0, 9);
    if (n >= 65 && n <= 70) n = map(n, 65, 70, 10, 15);
    if (n >= 97 && n <= 102) n = map(n, 97, 102, 10, 15);
    n = constrain(n, 0, 15);
    d = (d * 16) + n;
  }
  return d;
}


#ifdef DBGRUN
void dbgRun() {
  if (Serial.available()) {
    String s(Serial.readString());
    Serial.print(F("\n[!DBG!] "));
    Serial.println(s);

    Serial.print(F("Lift position: "));
    Serial.println((lift_position ? F("UP") : F("DOWN")));

    Serial.print(F("Minuts now: "));
    Serial.println((TD.tm_hour * 60 + TD.tm_min));

    Serial.print(F("Time\tOpen: "));
    Serial.print(TOPN);
    Serial.print(F("\tClose: "));
    Serial.println(TCSE);
    Serial.print(F("Time period: "));
    Serial.println((checkTimePeriod(TOPN, TCSE) ? F("IN") : F("OUT")));

    Serial.print(F("Weather\tWind: "));
    Serial.print(weather.W);
    Serial.print(F(" m/s\tRain: "));
    Serial.println((weather.R ? F("Yes") : F("No")));

    Serial.print(F("Sensor\tTin: "));
    Serial.print(Tin, 1);
    Serial.print(F("\tTout: "));
    Serial.println(Tout, 1);

    Serial.print(F("Gisteresis Tin - Tout: "));
    Serial.print((Tin - Tout), 1);
    Serial.print(F(" >= "));
    Serial.print(TGIST);
    Serial.print(F(" | "));
    Serial.println(((Tin - Tout) >= TGIST ? F("True") : F("False")));

    Serial.println(F("\n[!DBG!] END"));
  }
}
#endif
