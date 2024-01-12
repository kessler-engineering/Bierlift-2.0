void reley_handle() {
  if (releyStatus) {

    if (releyCommandStatus) {
      if (millis() - releyCommandProtTimer >= COMMAND_PROT_TIME) {
        if (SUSE && dfInit && !effMute) {
          dfPlayer.stop();
          dfPlayer.volume(SVOL);
          dfPlayer.play(mp3StrPars(crm.var(releyDirection ? "SUP" : "SDOWN")));
        }
        digitalWrite(releyDirection ? UP_PIN : DOWN_PIN, HIGH);
        ledOnBoard(1);
        optoStatus = true;
        optoTimer = millis();
        if (BUSE && blynkConnSt) {
          xSemaphoreTake(wifi_mutex, portMAX_DELAY);
          if (releyDirection) Blynk.virtualWrite(V0, (motorSpeed ? 2 : 1)); //U
          else Blynk.virtualWrite(V0, (motorSpeed ? -2 : -1)); //D
          xSemaphoreGive(wifi_mutex);
        }
        crm.webUpdate("STATUS", workState());
        Serial.println(String(releyDirection ? "UP_PIN: " : "DOWN_PIN: ") + String(digitalRead(releyDirection ? UP_PIN : DOWN_PIN)));
        releyCommandStatus = false;
      }
    }

    if ((millis() - releyOffTimer >= (releyDirection ? UTIME : DTIME)) || optosensor()) {
      releyAction(0);
      if (!releyDirection) {
        ledPowerAction(false);
        lift_position = false;
        if (SAVE_LIFT_STATUS) crm.var("LPOS", String(lift_position), 1);
        nCoolStatus = false;
        if (SUSE && dfInit) {
          dfPlayer.stop();
          dfPlayer.volume(0);
        }
      }
      releyDirection = !releyDirection;
    } else if (OLD) pwm();

  }
}


void releyAction (const int action) {
  Serial.println(String("\nACTION: ") + String(action));
  if (lastAction != action) {
    releyOffTimer = millis();
    optoStatus = false;

    if (action) dev_high(action);

    if (action > 0) {
      lift_position = true;
      if (mLED) ledPowerAction(true);
    }

    if (releyStatus) {
      digitalWrite(UP_PIN, LOW);
      digitalWrite(DOWN_PIN, LOW);
      ledOnBoard(0);
      releyCommandProtTimer = millis();
    }

    switch (action) {
      case 2: {
          motorSpeed = true;
          digitalWrite(SEED_PIN, (SINV ? LOW : HIGH));
          releyStatus = true;
          releyCommandStatus = true;
          releyDirection = true;
          Serial.println("Rise 2x UP");
          crm.webUpdate("STATUS", workState());
          crm.webNotif("i", lng(L_LIFT_UP2), 5);
          tgNotif(lng(L_LIFT_UP2));
          mqttSend("s", lng(L_LIFT_UP2));
        } break;
      case 1: {
          if (motorSpeed) {
            digitalWrite(SEED_PIN, (SINV ? HIGH : LOW));
            motorSpeed = false;
          }
          releyStatus = true;
          releyCommandStatus = true;
          releyDirection = true;
          Serial.println("Rise UP");
          crm.webUpdate("STATUS", workState());
          crm.webNotif("i", lng(L_LIFT_UP), 5);
          tgNotif(lng(L_LIFT_UP));
          mqttSend("s", lng(L_LIFT_UP));
        } break;
      case 0: {
          if (OLD) ledcWrite(0, (pwm_vol = 0));
          digitalWrite(UP_PIN, LOW);
          digitalWrite(DOWN_PIN, LOW);
          digitalWrite(SEED_PIN, (SINV ? HIGH : LOW));
          dev_low();
          ledOnBoard(0);
          releyStatus = false;
          releyCommandStatus = false;
          motorSpeed = false;
          effMute = false;
          if (BUSE && blynkConnSt) {
            //xSemaphoreTake(wifi_mutex, portMAX_DELAY);
            Blynk.virtualWrite(V0, 0);
            //xSemaphoreGive(wifi_mutex);
          }
          Serial.println("Rise STOP");
          crm.webUpdate("STATUS", workState());
          crm.webNotif("i", lng(L_LIFT_STOP), 5);
          tgNotif(lng(L_LIFT_STOP));
          mqttSend("s", lng(L_LIFT_STOP));
        } break;
      case -1: {
          if (motorSpeed) {
            digitalWrite(SEED_PIN, (SINV ? HIGH : LOW));
            motorSpeed = false;
          }
          releyStatus = true;
          releyCommandStatus = true;
          releyDirection = false;
          Serial.println("Rise DOWN");
          crm.webUpdate("STATUS", workState());
          crm.webNotif("i", lng(L_LIFT_DOWN), 5);
          tgNotif(lng(L_LIFT_DOWN));
          mqttSend("s", lng(L_LIFT_DOWN));
        } break;
      case -2: {
          motorSpeed = true;
          digitalWrite(SEED_PIN, (SINV ? LOW : HIGH));
          releyStatus = true;
          releyCommandStatus = true;
          releyDirection = false;
          Serial.println("Rise 2x DOWN");
          crm.webUpdate("STATUS", workState());
          crm.webNotif("i", lng(L_LIFT_DOWN2), 5);
          tgNotif(lng(L_LIFT_DOWN2));
          mqttSend("s", lng(L_LIFT_DOWN2));
        } break;
    }
    lastAction = action;
  }
}


void pwm() {
  static uint32_t pwm_timer = 0;
  if (pwm_vol < 254 && millis() - pwm_timer > 15) {
    pwm_timer = millis();
    pwm_vol += 3;
    ledcWrite(0, pwm_vol);
  }
}
