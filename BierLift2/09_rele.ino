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
        // Nicht auf den wifi_mutex warten (blockierte sonst loop() bei hängendem Blynk);
        // der Blynk-Task sendet den Wert.
        if (BUSE && blynkConnSt) {
          if (releyDirection) blynkV0Pending = (motorSpeed ? 2 : 1); //hoch
          else blynkV0Pending = (motorSpeed ? -2 : -1); //runter
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


// Einstiegspunkt für alle Fahrbefehle (2, 1, 0, -1, -2, LIFT_CMD_TOGGLE).
// Aufrufe aus loop() werden direkt ausgeführt, alle anderen Tasks/Ticker
// stellen den Befehl in die Queue – so greift nur loop() auf Relais und Motorzustand zu.
void releyAction(int action) {
  if (action != LIFT_CMD_TOGGLE && (action < -2 || action > 2)) {
    Serial.println(String("[LIFT] Invalid command: ") + String(action));
    return;
  }
  if (loopTask_h == NULL || xTaskGetCurrentTaskHandle() == loopTask_h) {
    liftCmdExec(action);
  } else if (liftCmdQueue == NULL || xQueueSend(liftCmdQueue, &action, 0) != pdTRUE) {
    Serial.println(F("[LIFT] Command queue full, command dropped"));
  }
}


void liftCmdQueueHandle() {
  int a;
  while (liftCmdQueue != NULL && xQueueReceive(liftCmdQueue, &a, 0) == pdTRUE) {
    liftCmdExec(a);
  }
}


void liftCmdExec(int action) {
  if (action == LIFT_CMD_TOGGLE) {
    if (releyStatus) {
      releyActionExec(0);
      releyDirection = !releyDirection;
    } else releyDirection ? releyActionExec(1) : releyActionExec(-1);
  } else releyActionExec(action);
}


// Nur aus loop() aufrufen (über releyAction / liftCmdExec).
void releyActionExec(const int action) {
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
          if (BUSE && blynkConnSt) blynkV0Pending = 0;   // sendet der Blynk-Task
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
