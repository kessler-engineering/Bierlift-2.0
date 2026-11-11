/*
  MQTT-Topics ([Präfix/] ist optional, Einstellung "Präfix", endet mit "/")

  Empfangen (Befehle an den Lift; retained-Nachrichten werden ignoriert):
  [Präfix/]BierLift/a          - Fahren: 2 = schnell hoch, 1 = hoch, 0 = Stopp, -1 = runter, -2 = schnell runter
  [Präfix/]BierLift/t          - leere Nachricht: Temperaturen als Text an .../t senden
  [Präfix/]BierLift/s          - leere Nachricht: Status als Text an .../s senden
  [Präfix/]BierLift/chprot     - Kindersicherung: 1 = ein, 0 = aus
  [Präfix/]BierLift/gpio22     - Development-Pin 22: 1 = HIGH, 0 = LOW (nur wenn Pin aktiviert)
  [Präfix/]BierLift/gpio25     - Development-Pin 25: 1 = HIGH, 0 = LOW
  [Präfix/]BierLift/gpio26     - Development-Pin 26: 1 = HIGH, 0 = LOW

  Senden (vom Lift, retained):
  [Präfix/]BierLift/s          - Status- und Ereignismeldungen (Text)
  [Präfix/]BierLift/t/in       - Temperatur innen, °C
  [Präfix/]BierLift/t/out      - Temperatur außen, °C
  [Präfix/]BierLift/w          - Wasserstand: 1 = hoch, 0 = niedrig
*/


// AsyncMqttClient speichert für Server/Login/Passwort nur Zeiger – die Strings müssen
// also dauerhaft leben. Früher wurde dafür bei jedem (Re-)Connect neuer Speicher
// reserviert und nie freigegeben (Speicherleck). Jetzt feste Puffer.
static String mqHost, mqUser, mqPass;

String mqttBaseTopic() {
  return crm.var("MQTP") + BL_NAME;
}


void OnMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  String t = topic;
  String tt = crm.var("MQTP") + BL_NAME;
  SP("[MQTT] Message for topic \"");
  SP(t);
  String data = String();
  for (size_t i = 0; i < len; ++i) {
    data += payload[i];
  }
  SPLN("\": " + data);

  // Gespeicherte (retained) Nachrichten nie als Befehl ausführen: ein retained "1" auf /a
  // ließe den Lift sonst nach jedem Neustart/Reconnect hochfahren.
  if (properties.retain) {
    SPLN(F("[MQTT] Retained message ignored."));
    return;
  }

  if (t == (tt + "/a")) {
    releyAction(data.toInt());
  } else if (t == (tt + "/t") && data == "") {
    mqttSend("t", tempState());
  } else if (t == (tt + "/s") && data == "") {
    mqttSend("s", workState());
  } else if (t == (tt + "/chprot")) {
    String d = data;
    crm.var("CHP", (d.toInt() ? "true" : "false"));
    crm.webUpdate("CHP", (d.toInt() ? "true" : "false"));
  } else if (DP22 && t == (tt + "/gpio22")) {
    digitalWrite(DEV_PIN_22, data.toInt());
  } else if (DP25 && t == (tt + "/gpio25")) {
    digitalWrite(DEV_PIN_25, data.toInt());
  } else if (DP26 && t == (tt + "/gpio26")) {
    digitalWrite(DEV_PIN_26, data.toInt());
  }
}

void onMqttConnect(bool sessionPresent) {
  SPLN("[MQTT] Connected to " + crm.var("MQSRV"));
  mqttClient.subscribe((mqttBaseTopic() + "/#").c_str(), 1);   // Topic wird beim Aufruf kopiert
  SPLN("[MQTT] Subscribe on \"" + crm.var("MQTP") + BL_NAME + "/#\", QoS: 1");
  mqttSend("s", lng(L_TELEGA_ONLINE));
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  SPLN("[MQTT] Disconnected from " + crm.var("MQSRV"));
  if (MQUSE) mqttTick.once_ms(2000, mqttConnect);
}


void mqttInit(bool a) {
  static bool i = false;
  if (a) {
    if (!i) {
      i = true;
      mqttClient.onMessage(OnMqttMessage);
      mqttClient.onConnect(onMqttConnect);
      mqttClient.onDisconnect(onMqttDisconnect);
      // static: der Client speichert nur den Zeiger (vorher zeigte er auf einen gelöschten String)
      static String id = crm.uint64ToString(ESP.getEfuseMac());
      mqttClient.setClientId(id.c_str());
    }
    //mqttClient.setWill(strToChr(crm.var("MQTP") + BL_NAME + "/s"), 1, false, "BierLift offline");
    // Nur hier setzen, wenn der Client getrennt ist (sonst könnten alte Zeiger ungültig werden).
    mqHost = crm.var("MQSRV");
    mqUser = crm.var("MQLG");
    mqPass = crm.var("MQPW");
    mqttClient.setCredentials(mqUser.c_str(), mqPass.c_str());
    mqttClient.setServer(mqHost.c_str(), crm.var("MQPRT").toInt());
    mqttConnect();
  } else {
    mqttClient.unsubscribe((mqttBaseTopic() + "/#").c_str());
    mqttTick.detach();
    mqttClient.disconnect(true);
    mqttClient.clearQueue();
  }
}


void mqttConnect() {
  if (MQUSE) {
    if (WiFi.status() == WL_CONNECTED) {
      SPLN("[MQTT] Try connect to " + crm.var("MQSRV"));
      mqttClient.connect();
    } else mqttTick.once_ms(2000, mqttConnect);
  }
}


void mqttSend(String t, String d) {
  if (MQUSE && mqttClient.connected()) {
    String topic = crm.var("MQTP") + BL_NAME + "/" + t;
    uint16_t count = mqttClient.publish(topic.c_str(), 1, true, String(d).c_str());
    Serial.println("[MQTT] Send in \"" + topic + "\" message [" + String(count) + "]: " + String(d));
  }
}

/*String topic_dicr() {
  String pt = crm.var("MQTP") + BL_NAME + "/";

  String t = "Lift steuern: <b style='color:#0f0;'>";
  t += pt + "a</b><br>";
  t += "SCHNELL HOCH = 2; HOCH = 1; STOPP = 0; RUNTER = -1; SCHNELL RUNTER = -2<br><br>";

  t += "Temperatur: <b style='color:#0f0;'>";
  t += pt + "t</b><br>";
  t += "Antwort im Topic .../t<br><br>";

  t += "Status: <b style='color:#0f0;'>";
  t += pt + "s</b><br>";
  t += "Anfrage muss leer sein.";
  return t;
  }*/
