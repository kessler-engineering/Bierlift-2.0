
void button_hw() {
  bt.run();
  if (bt.getState()) {
    if (bt.isHolded()) {
      crm.webNotif("i", lng(L_RESET), 5);
      tgNotif(lng(L_RESET));
      mqttSend("s", lng(L_RESET));
      crm.cfgDelete();
    }


    if (bt.hasClicks(1) && !CHP) {
      if (releyStatus) {
        releyAction(0);
        releyDirection = !releyDirection;
      } else {
        effMute = false;
        releyDirection ? releyAction(1) : releyAction(-1);
      }
    }

    if (bt.hasClicks(2) && !CHP) {
      effMute = false;
      releyDirection ? releyAction(2) : releyAction(-2);
    }

    if (bt.hasClicks(3) && !CHP) {
      Serial.println(F("[WiFi] Reconnect to wifi..."));
      WiFi.reconnect();
    }

    if (bt.hasClicks(5)) {
      crm.webNotif("i", lng(L_NOTIF_REBOOT), 5);
      tgNotif(lng(L_NOTIF_REBOOT));
      mqttSend("s", lng(L_NOTIF_REBOOT));
      crm.espReboot();
    }
  }
}

void button_web() {
  if (crm.btnSwState()) {
    if (OLD == false) {
      crm.btnCallback("UP2", UP2);
      crm.btnCallback("DN2", DN2);
      if (USEPM) crm.btnCallback("PUMP", PUMPact);
    }
    crm.btnCallback("RBT", REBOOT);
    crm.btnCallback("UP1", UP1);
    crm.btnCallback("STP", STP);
    crm.btnCallback("DN1", DN1);
    crm.btnCallback("BTCUDT", checkUpdateWeb);
    if (SUSE) {
      crm.btnCallback("PSD", playPreviousDemo);
      crm.btnCallback("PD", playDemo);
      crm.btnCallback("SD", stopDemo);
      crm.btnCallback("NSD", playNextDemo);
    }
    dev_bt();
  }
}



void UP2() {
  effMute = false;
  releyAction(2);
}

void UP1() {
  effMute = false;
  releyAction(1);
}

void STP() {
  if (SUSE && dfInit && !releyStatus && lift_position) {
    dfPlayer.stop();
    dfPlayer.volume(0);
  }
  releyAction(0);
}

void DN1() {
  effMute = false;
  releyAction(-1);
}

void DN2() {
  effMute = false;
  releyAction(-2);
}


void PUMPact() {
  pumpStatus ? pump_action(0) : pump_action(1);
}


void playPreviousDemo() {
  if (dfInit) {
    int n = crm.var("SDEMO").toInt();
    if (n > 1) {
      n--;
      //crm.webUpdate("SDEMO", String(n), true);
      crm.var("SDEMO", n, false);
    }
    dfPlayer.stop();
    dfPlayer.volume(SVOL);
    dfPlayer.play(n);
  }
}

void playDemo() {
  if (dfInit) {
    dfPlayer.stop();
    dfPlayer.volume(SVOL);
    dfPlayer.play(crm.var("SDEMO").toInt());
  }
}

void stopDemo() {
  if (dfInit) {
    dfPlayer.stop();
    dfPlayer.volume(0);
  }
}

void playNextDemo() {
  if (dfInit) {
    int n = crm.var("SDEMO").toInt();
    if (n < fileCounts) {
      n++;
      crm.var("SDEMO", n, false);
    }
    dfPlayer.stop();
    dfPlayer.volume(SVOL);
    dfPlayer.play(n);
  }
}


void REBOOT() {
  crm.webNotif("i", lng(L_NOTIF_REBOOT), 3);
  tgNotif(lng(L_NOTIF_REBOOT));
  mqttSend("s", lng(L_NOTIF_REBOOT));
  crm.espReboot();
}
