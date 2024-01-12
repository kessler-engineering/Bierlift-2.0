/*
  Управление лифтом
  [Prefix/]BierLift/a                - ВВЕРХ_2x = 2; ВВЕРХ = 1; СТОП = 0; ВНИЗ = -1; ВНИЗ_2x = -2

  Температура
  [Prefix/]BierLift/t                - Ответ в топик .../s

  Статус
  [Prefix/]BierLift/s                - Запрос должен быть пустым.

  Детская защита
  [Prefix/]BierLift/chprot           - 1 = Enable, 0 = Disable

  Пин22
  [Prefix/]BierLift/gpio22            - 1 = HIGH, 0 = LOW
  Пин25
  [Prefix/]BierLift/gpio25            - 1 = HIGH, 0 = LOW
  Пин26
  [Prefix/]BierLift/gpio26            - 1 = HIGH, 0 = LOW


*/


char* strToChr(String s) {
  char* S = new char[s.length() + 1];
  strcpy(S, s.c_str());
  return S;
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
  mqttClient.subscribe(strToChr(crm.var("MQTP") + BL_NAME + "/#"), 1);
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
      String id = crm.uint64ToString(ESP.getEfuseMac());
      mqttClient.setClientId(id.c_str());
    }
    //mqttClient.setWill(strToChr(crm.var("MQTP") + BL_NAME + "/s"), 1, false, "BierLift offline");
    mqttClient.setCredentials(strToChr(crm.var("MQLG")), strToChr(crm.var("MQPW")));
    mqttClient.setServer(strToChr(crm.var("MQSRV")), crm.var("MQPRT").toInt());
    mqttConnect();
  } else {
    mqttClient.unsubscribe(strToChr(crm.var("MQTP") + BL_NAME + "/#"));
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

  String t = "Управление лифтом: <b style='color:#0f0;'>";
  t += pt + "a</b><br>";
  t += "ВВЕРХ1 = 2; ВВЕРХ = 1; СТОП = 0; ВНИЗ = -1; ВНИЗ2 = -2<br><br>";

  t += "Температура: <b style='color:#0f0;'>";
  t += pt + "t</b><br>";
  t += "Ответ в топик .../s<br><br>";

  t += "Статус: <b style='color:#0f0;'>";
  t += pt + "s</b><br>";
  t += "Запрос должен быть пустым.";
  return t;
  }*/
