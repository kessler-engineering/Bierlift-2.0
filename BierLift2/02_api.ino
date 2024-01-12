
void api(String payload) {
  Serial.print("[API] ");
  Serial.println(payload);

  xSemaphoreTake(wifi_mutex, portMAX_DELAY);
  DynamicJsonDocument doc(200);
  deserializeJson(doc, payload);

  const char* action = doc["action"];
  if (action != NULL) {
    switch (String(action).toInt()) {
      case 2: if (OLD == false) releyAction(2); break;
      case 1: releyAction(1); break;
      case 0: releyAction(0); break;
      case -1: releyAction(-1); break;
      case -2: if (OLD == false) releyAction (-2); break;
    }
    crm.apiResponse("action", action);
  }

  const char* state = doc["state"];
  if (state != NULL) crm.apiResponse("state", workState());

  const char* t = doc["t"];
  if (t != NULL) {
    crm.apiResponse("t1", String(Tin));
    crm.apiResponse("t2", String(Tout));
  } else {
    const char* t1 = doc["t1"];
    if (t1 != NULL) crm.apiResponse("t1", String(Tin));

    const char* t2 = doc["t2"];
    if (t2 != NULL) crm.apiResponse("t2", String(Tout));
  }

  const char* sendCode = doc["sendCode"];
  if (sendCode != NULL) {
    const char* sendPort = doc["sendPort"];
    const char* sendPacs = doc["sendPacs"];
    long code = String(sendCode).toInt();
    uint8_t port = String(sendPort).toInt();
    uint8_t pacs = String(sendPacs).toInt();
    if (!port) port = 22;
    if (!pacs) pacs = 40;
    crm.apiResponse("sendCode", String(code));
    crm.apiResponse("sendPort", String(port));
    crm.apiResponse("sendPacs", String(pacs));
    SendCode(code, port, pacs);
  }

  const char* getvar = doc["getvar"];
  if (getvar != NULL) {
    String v = getvar;
    crm.apiResponse("var", crm.var(v));
  }

  xSemaphoreGive(wifi_mutex);
}
