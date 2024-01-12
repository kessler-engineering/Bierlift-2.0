
void dev_init() {
  pinMode(DEV_PIN_22, OUTPUT);
  digitalWrite(DEV_PIN_22, DP22S);
  pinMode(DEV_PIN_25, OUTPUT);
  digitalWrite(DEV_PIN_25, DP25S);
  pinMode(DEV_PIN_26, OUTPUT);
  digitalWrite(DEV_PIN_26, DP26S);
  if (crm.var("DP22N") == "null") crm.var("DP22N", "GPIO22");
  if (crm.var("DP25N") == "null") crm.var("DP25N", "GPIO25");
  if (crm.var("DP26N") == "null") crm.var("DP26N", "GPIO26");
}


void devPinStatus(byte p) {
  Serial.printf("[DEV] Pin %d status: %d\n", p, digitalRead(p));
}


void dev_update() {
  //PIN 22
  DP22 = crm.var("DP22") == "true";
  DP22U = crm.var("DP22U") == "true";
  DP22NO = crm.var("DP22NO") == "true";
  DP22D = crm.var("DP22D") == "true";
  DP22C = crm.var("DP22C") == "true";
  DP22S = crm.var("DP22S") == "true";

  //PIN 25
  DP25 = crm.var("DP25") == "true";
  DP25U = crm.var("DP25U") == "true";
  DP25NO = crm.var("DP25NO") == "true";
  DP25D = crm.var("DP25D") == "true";
  DP25C = crm.var("DP25C") == "true";
  DP25S = crm.var("DP25S") == "true";

  //PIN 26
  DP26 = crm.var("DP26") == "true";
  DP26U = crm.var("DP26U") == "true";
  DP26NO = crm.var("DP26NO") == "true";
  DP26D = crm.var("DP26D") == "true";
  DP26C = crm.var("DP26C") == "true";
  DP26S = crm.var("DP26S") == "true";
}


void dev_high(const int a) {
  if (DP22 && (DP22U && a > 0 || DP22D && a < 0)) {
    digitalWrite(DEV_PIN_22, !DP22S);
    crm.webUpdate("DP22B", "  " + crm.var("DP22N"));
    devPinStatus(DEV_PIN_22);
  }
  if (DP25 && (DP25U && a > 0 || DP25D && a < 0)) {
    digitalWrite(DEV_PIN_25, !DP25S);
    crm.webUpdate("DP25B", "  " + crm.var("DP25N"));
    devPinStatus(DEV_PIN_25);
  }
  if (DP26 && (DP26U && a > 0 || DP26D && a < 0)) {
    digitalWrite(DEV_PIN_26, !DP26S);
    crm.webUpdate("DP26B", "  " + crm.var("DP26N"));
    devPinStatus(DEV_PIN_26);
  }
}


void dev_low() {
  if (DP22 && ((DP22U && releyDirection && !DP22NO) || (DP22D && !releyDirection))) {
    digitalWrite(DEV_PIN_22, DP22S);
    crm.webUpdate("DP22B", "  " + crm.var("DP22N"));
    devPinStatus(DEV_PIN_22);
  }
  
  if (DP25 && ((DP25U && releyDirection && !DP25NO) || (DP25D && !releyDirection))) {
    digitalWrite(DEV_PIN_25, DP25S);
    crm.webUpdate("DP25B", "  " + crm.var("DP25N"));
    devPinStatus(DEV_PIN_25);
  }
  
  if (DP26 && ((DP26U && releyDirection && !DP26NO) || (DP26D && !releyDirection))) {
    digitalWrite(DEV_PIN_26, DP26S);
    crm.webUpdate("DP26B", "  " + crm.var("DP26N"));
    devPinStatus(DEV_PIN_26);
  }
}


void dev_bt() {
  if (DP22) crm.btnCallback("DP22B", devButt22);
  if (DP25) crm.btnCallback("DP25B", devButt25);
  if (DP26) crm.btnCallback("DP26B", devButt26);
}

void devButt22() {
  bool s = !digitalRead(DEV_PIN_22);
  digitalWrite(DEV_PIN_22, s);
  crm.webUpdate("DP22B", (s == DP22S ? "  " : "  ") + crm.var("DP22N"));
  devPinStatus(DEV_PIN_22);
}

void devButt25() {
  bool s = !digitalRead(DEV_PIN_25);
  digitalWrite(DEV_PIN_25, s);
  crm.webUpdate("DP25B", (s == DP25S ? "  " : "  ") + crm.var("DP25N"));
  devPinStatus(DEV_PIN_25);
}

void devButt26() {
  bool s = !digitalRead(DEV_PIN_26);
  digitalWrite(DEV_PIN_26, s);
  crm.webUpdate("DP26B", (s == DP26S ? "  " : "  ") + crm.var("DP26N"));
  devPinStatus(DEV_PIN_26);
}
