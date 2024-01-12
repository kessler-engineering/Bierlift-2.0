void led_handle() {
  if (brightnessStatus) {
    if (millis() - brTimer >= 15) {
      brTimer = millis();
      if (curBR == mainBR) brightnessStatus = false;
      else {
        curBR <= mainBR ? ++curBR : --curBR;
        FastLED.setBrightness(curBR);
      }
      if (curBR <= 0) {
        ledAnimateStatus = false;
        brightnessStatus = false;
        FastLED.clear();
      }
      FastLED.show();
    }
  }

  if (ledAnimateStatus) {
    if (millis() - ledTimer >= SPEED) {
      ledTimer = millis();
      switch (mLED) {
        case 0:  break;
        case 1: {
            setAll(CRGB(C1, C2, C3));
            ledAnimateStatus = false;
          } break;
        case 2: rainbow(); break;
        case 3: colors(); break;
        case 4: sparkles(); break;
        case 5: Glitter(); break;
        case 6: fire(INV); break;
        case 7: fire(INV); break;
        case 8: fire(INV); break;
        case 9: fire(INV); break;
        case 10: fire(INV); break;
        case 11: sinelon(); break;
        case 12: snowSparkle(); break;
      }
    }
    FastLED.show();
  }
}


void ledPowerAction(bool action) {
  if (action && !effMute) {
    randomSeed(millis());
    mainBR = crm.var("BR").toInt();
    while (!ledAnimateStatus) {
      ledAnimateStatus = true;
    }
    while (!brightnessStatus) {
      brightnessStatus = true;
    }
  } else {
    mainBR = 0;
    brightnessStatus = true;
  }
}


void setAll(CRGB newcolor) {
  for (uint16_t i = 0; i < LEDN; i++) {
    leds[i] = newcolor;
  }
}

uint32_t getPixColor(uint16_t thisPixel) {
  return (((uint32_t)leds[thisPixel].r << 16) | ((long)leds[thisPixel].g << 8 ) | (long)leds[thisPixel].b);
}

//********************* Е Ф Ф Е К Т Ы ***********************
//https://www.tweaking4all.com/hardware/arduino/adruino-led-strip-effects/
byte hue = 0;
CRGBPalette16 gPal;

//Радуга
void rainbow() {
  //fill_rainbow(leds, LEDN, (INV ? hue++ : hue--), 255 / LEDN);
  fill_rainbow(leds, LEDN, (INV ? hue++ : hue--), PARAM);
  FastLED.setTemperature(OvercastSky);
}

//Цвета
void colors() {
  setAll(CHSV((INV ? hue++ : hue--), 255, 255));
}

//Конфетти
void sparkles() {
  //fadeToBlackBy(leds, LEDN, 255 / LEDN);
  fadeToBlackBy(leds, LEDN, PARAM);
  byte thisNum = random(0, LEDN);
  if (getPixColor(thisNum) == 0) leds[thisNum] = CHSV(random(0, 255), 255, 255);
}

//Блеск
void Glitter() {
  //FastLED.clear();
  //fadeToBlackBy(leds, LEDN, 40);
  fadeToBlackBy(leds, LEDN, PARAM);
  for (byte i = 0; i < 15; i++) {
    if (random8() < LEDN) {
      leds[random16(LEDN)] += CRGB::White;
      FastLED.show();
    }
  }
}

//Огонь
void FireModeChange(byte fm) {
  switch (fm) {
    case 6: gPal = CRGBPalette16(CRGB::Black, CRGB::Orange, CRGB(255, 200, 0), CRGB(255, 100, 0)); break;
    case 7: gPal = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White); break;
    case 8: gPal = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua,  CRGB::White); break;
    case 9: gPal = CRGBPalette16(CRGB::Black, CRGB::Red, CRGB::White); break;
    case 10: gPal = CRGBPalette16(CRGB::Black, CRGB::Green, CRGB::Blue,  CRGB::White); break;
  }
}

void fire(bool revers) {
  static byte heat[200];
  for (int i = 0; i < LEDN; i++) {
    //heat[i] = qsub8(heat[i],  random8(0, 755 / LEDN));
    heat[i] = qsub8(heat[i],  random8(0, PARAM));
  }
  for (int k = LEDN - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) * 0.33;
  }
  if (random8() < 85) {
    int y = random8(LEDN * 0.2);
    heat[y] = qadd8(heat[y], random8(160, 255) );
  }
  for (int j = 0; j < LEDN; j++) {
    byte colorindex = scale8(heat[j], 240);
    CRGB color = ColorFromPalette(gPal, colorindex);
    int pixelnumber;
    if (revers) pixelnumber = (LEDN - 1) - j;
    else pixelnumber = j;
    leds[pixelnumber] = color;
  }
}

//Огонек
void sinelon() {
  //fadeToBlackBy(leds, LEDN, 500 / LEDN);
  fadeToBlackBy(leds, LEDN, PARAM);
  int pos = beatsin16(15, 0, LEDN - 1);
  leds[pos] += CHSV((INV ? hue++ : hue--), 255, 192);
}

//
void snowSparkle() {
  FastLED.clear();
  for (uint16_t i = 0; i < LEDN; i++) {
    //if (random8() < 5) leds[i] += CRGB::White;
    if (random8() < PARAM) leds[i] += CRGB::White;
    else leds[i] = CRGB(80, 80, 80);
  }
}


//********** On board led indication *************
void ledFlash(void *pvParameters) {
  int kucha = uxTaskGetStackHighWaterMark(NULL);
  pinMode(LED_ONBOARD_PIN, OUTPUT);
  digitalWrite(LED_ONBOARD_PIN, LOW);
  for (;;) {
    if (crm.connState) {
      int m = WiFi.getMode();
      if (m == 1) {
        ledFlashAct(50);
        ledFlashAct(3000);
      } else if (m == 2) {
        ledFlashAct(50);
        ledFlashAct(50);
        ledFlashAct(50);
        ledFlashAct(3000);
      } else if (m == 3) {
        ledFlashAct(50);
        ledFlashAct(50);
        ledFlashAct(50);
        ledFlashAct(50);
        ledFlashAct(50);
        ledFlashAct(3000);
      } else vTaskDelay(1000);
    } else {
      ledFlashAct(100);
      ledFlashAct(100);
    }
    if (kucha > uxTaskGetStackHighWaterMark(NULL))                //TEST
      Serial.println("[HEAP] ledFlash: " + String(kucha = uxTaskGetStackHighWaterMark(NULL)));
  }
}


void ledOnBoard(byte a) {
  if (LFLASH) digitalWrite(LED_ONBOARD_PIN, a);
}


void ledFlashAct(uint16_t t) {
  digitalWrite(LED_ONBOARD_PIN, !digitalRead(LED_ONBOARD_PIN));
  vTaskDelay(t);
}
