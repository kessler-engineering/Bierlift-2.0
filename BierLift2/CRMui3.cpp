#include "CRMui3.h"

#include "web_index.html.h"
#include "web_rf.html.h"
#include "web_script.js.h"
#include "web_style.css.h"
#include "web/notif.js.h"
#include "web/notif.css.h"
//#include "web/chart.js.h"
#include "web/3ds.ttf.h"
#include "web/fonticon.woff2.h"
#include "web/favicon.ico.h"
#include "web/icon-192x192.png.h"


AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
DNSServer dnsServer;


void CRMui3::useArduinoOta() {
  _useArduinoOta = true;
}

void CRMui3::begin(const String &app_name, void (*uiFunction)(), void (*updateFunction)(), void (*apiFunction)(String), uint32_t baud) {
  if (baud > 0) {
    Serial.begin(baud);
    Serial.setTimeout(100);
    Serial.flush();
    _debug = true;
  }
  SPLN(String(F("\nBierlift ver:")) + CRM_VER);
  if (uiFunction == NULL) {
    SPLN(F("[ERROR] uiFunction not defined."));
    return;
  }
  ui = uiFunction;
  if (updateFunction != NULL) upd = updateFunction;
  if (apiFunction != NULL) api = apiFunction;
  _app_name = app_name;
  cfgLoad();
  http();
  wifiStart();
  server.begin();
  if (upd) upd();
  if (_useArduinoOta) {
    ArduinoOTA.setHostname(var(F("_as")).c_str());
    ArduinoOTA.begin();
  }
  if (_wifiMode > 1) dnsServer.start(53, "*", WiFi.softAPIP());
  _start = false;
}


void CRMui3::run() {
  if (_useArduinoOta) ArduinoOTA.handle();
  if (millis() - _runTimer >= 1000) {
    _runTimer = millis();
    _upTime++;
    if (_wifiMode > 1) dnsServer.processNextRequest();
    cfgAutoSave();
    if (_sendingToWeb) {
      _sendingToWeb = webConnCountStatus();
      if (WiFi.getMode() == 2 && !WiFi.softAPgetStationNum()) _sendingToWeb = false;
      webUpdate("uptime", upTime());
      webUpdate("wifi", String(WiFi.RSSI()));
      webUpdate("ram", String(ESP.getFreeHeap()));
      webUpdate("devip", (WiFi.getMode() == 2 ? WiFi.softAPIP().toString() : WiFi.localIP().toString()), true);
    }
    if (_espNeedReboot) espReboot();
  }
}


String CRMui3::upTime(bool s) {
  static bool upTimeInit = true;
  String b = String();
  if (upTimeInit) {
    _upTime += millis() / 1000 - _upTime;
    upTimeInit = false;
  }
  if (s) return String(_upTime);
  else {
    if ((_upTime / 86400 % 365) != 0) {
      b += _upTime / 86400 % 365;
      b += F(",  ");
    }
    b += _upTime / 3600 % 24;
    b += F(":");
    if ((_upTime / 60 % 60) < 10) b += F("0");
    b += _upTime / 60 % 60;
    b += F(":");
    if ((_upTime % 60) < 10) b += F("0");
    b += _upTime % 60;
    return b;
  }
}


void CRMui3::defaultWifi(uint8_t mode, const String &ap_ssid, const String &ap_pass, const String &ssid, const String &pass, long wtc) {
  if (var(F("_wm")) == F("null")) var(F("_wm"), (((mode == 1 || mode == 3) && ssid == "") ? 2 : mode));
  if (var(F("_as")) == F("null")) {
    String _id = uint64ToString(ESP.getEfuseMac());
    var(F("_as"), (ap_ssid == "" ? String(F("CRMui3-")) + _id : ap_ssid));
  }
  if (var(F("_ap")) == F("null")) var(F("_ap"), ap_pass);
  if (var(F("_s")) == F("null")) var(F("_s"), ssid);
  if (var(F("_p")) == F("null")) var(F("_p"), pass);
  if (var(F("wtc")) == F("null")) var(F("wtc"), wtc);
}


void CRMui3::http() {
  ws.onEvent([this](AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      _sendingToWeb = true;
      ws.cleanupClients();
      //client->ping();
      DBGLN(String(F("[WS] ID ")) + String(client->id()) + F(" Connect"));
    } else if (type == WS_EVT_DISCONNECT) {
      DBGLN(String(F("[WS] ID ")) + String(client->id()) + F(" Disconnect"));
      if (ws.count() < 1) _sendingToWeb = false;
    } else if (type == WS_EVT_ERROR) {
      DBGLN(String(F("[WS] ID ")) + String(client->id()) + F(" Error"));
    } /*else if (type == WS_EVT_DATA) {
      AwsFrameInfo *info = (AwsFrameInfo*)arg;
      if (info->final && info->index == 0 && info->len == len) {
        if (info->opcode == WS_TEXT) { // если текстовые данные вебсокета
          data[len] = 0;
          if (String((char*)data) == "i") {

          }
        }
      }
    }*/
  });

  server.addHandler(&ws);



  server.on("/ui", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    String _id = uint64ToString(ESP.getEfuseMac());
    _buf = String(F("{\"_t\":0,\"an\":\"")) + _app_name + F("\",\"id\":\"") + _id + F("\",\"fw\":\"") +
           CRM_VER + F("\",\"a\":") + String(_AuthenticateStatus) + F(",\"c\":[");
    ui();
    _buf += F("],\"cfg\":");
    serializeJson(cfg, _buf);
    _buf += "}";
    if (_buf.length() > 7000) request->send_P(200, F("application/json"), _buf.c_str());
    else request->send(200, F("application/json"), _buf);
    _buf = String();
  });


  server.on("/set", HTTP_ANY, [this](AsyncWebServerRequest * request) {   //HTTP_ANY
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send(200);
    int headers = request->params();
    for (int i = 0; i < headers; i++) {
      AsyncWebParameter* p = request->getParam(i);
      String pname = p->name();
      if (pname.indexOf("BT_") != -1) _btnui = pname.substring(3);
      else if (pname == "wUPD") ws.textAll(String("{\"_t\":0}").c_str());
      else {
        DBGLN("[VAR] " + pname + F(" = ") + p->value());
        if (pname.indexOf("CR_") != -1) {
          pname = pname.substring(3);
          var(pname, p->value(), false);
        } else var(pname, p->value());
        if (webConnCountStatus() > 1) {
          ws.textAll(String("{\"_t\":2,\"d\":[[\"" + pname + "\",\"" + p->value() + "\"]]}").c_str());
        }
        if (upd) upd();
      }
    }
  });


  if (api) {
    server.on("/api", HTTP_GET, [this](AsyncWebServerRequest * request) {
      if (_AuthenticateStatus) {
        if (_apiKey == "") {
          if (!request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
            return request->requestAuthentication();
        } else {
          if (request->getParam(0)->value() != _apiKey)
            return request->requestAuthentication();
        }
      }
      uint32_t t = micros();
      String s = "{";
      for (int i = 0; i < request->params(); i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (i) s += ",";
        s += "\"" + String(p->name()) + F("\":\"") + String(p->value()) + F("\"");
      }
      s += "}";
      _apiResponse = "{\"stateRequest\":\"OK\",\"leadTime_ms\":\"" + String(((float)(micros() - t) / 1000), 3) + "\"}";
      api(s);
      request->send(200, F("application/json"), _apiResponse );
      _apiResponse = String();
    });
  }


  server.on("/reset", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send(200, F("text/plain"), F("ESP Reset. Device reboot..."));
    request->client()->close();
    cfgDelete();
  });


  server.on("/cfgdownload", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    cfgSave();
    if (SPIFFS.exists(F("/config.json"))) {
      File cf = SPIFFS.open(F("/config.json"), "r");
      String s = cf.readString();
      cf.close();
      AsyncWebServerResponse *response = request->beginResponse(200, F("application/json"), s);
      response->addHeader(F("Content-Disposition"), F("attachment; filename=\"BierLift.cfg\""));
      request->send(response);
    } else request->send(404, F("text/plain"), F("Configuration file not found."));
  });


  server.on("/cfgupload", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, F("text/html"), F("ERROR, go back and try again."));
    response->addHeader(F("Connection"), F("close"));
    request->send(response);
  }, [this](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();

    if (request->header(F("Content-Length")).toInt() > 5140) {
      request->send(413, F("text/plain"), F("Aborted upload because file size too big."));
      request->client()->close();
      Serial.println(String(F("File size ")) + filename + F(" too big."));
      return;
    }

    static String s = String();
    for (size_t i = 0; i < len; i++) {
      s += (char)data[i];
    }

    if (final) {
      Serial.println(String(F("File ")) + filename + F(" upload."));
      if (s.startsWith(F("{")) && s.endsWith(F("}"))) {
        DynamicJsonDocument doc(4096);
        deserializeJson(doc, s);
        s = String();
        for (JsonPair kv : doc.as<JsonObject>()) {
          var(String(kv.key().c_str()), kv.value().as<String>());
        }
        if (upd) upd();
        request->send(200, F("text/plain"), F("Config file update."));
        Serial.println(String(F("Configuratoin from ")) + filename + F(" write to SPIFFS."));
      } else {
        s = String();
        request->send(200, F("text/plain"), F("Configuration file content does not meet requirements."));
        Serial.println(String(F("Content into ")) + filename + F(" file, does not meet requirements."));
      }
    }
  });


  server.on("/update", HTTP_POST, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse(200, F("text/html"), F("ERROR, go back and try again."));
    response->addHeader(F("Connection"), F("close"));
    request->send(response);
  }, [this](AsyncWebServerRequest * request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
    if (!index) {
      SPLN(String(F("Update started, please wait.\nFirmware file: ")) + filename);
      if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) Update.printError(Serial);
    }
    if (!Update.hasError() && Update.write(data, len) != len) Update.printError(Serial);
    if (final) {
      if (Update.end(true)) {
        SPLN(F("Update success."));
        _espNeedReboot = !Update.hasError();
        request->send(200, F("text/plain"), _espNeedReboot ? F("UPDATE SUCCESS!") : F("UPDATE FAILED!"));
      } else {
        request->send(200, F("text/plain"), F("UPDATE ERROR! Please repeat."));
        Update.printError(Serial);
      }
    }
  });


  server.on("/wifi", HTTP_ANY, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    auto s = wifiScan();
    SPLN(s);
    request->send_P(200, "application/json", s.c_str());
  });


  // ********** Page files **********
  server.on("/", HTTP_ANY, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send_P(200, F("text/html"), index_html);
  });

  server.on("/rf", HTTP_ANY, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send_P(200, F("text/html"), rf_html);
  });

  server.on("/script.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send_P(200, F("application/javascript"), script_js);
  });

  server.on("/style.css", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send_P(200, F("text/css"), style_css);
  });

  server.on("/notif.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), notif_js, notif_js_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  server.on("/notif.css", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("text/css"), notif_css, notif_css_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  /*
    Server.on("/chart.js", HTTP_GET, [this](AsyncWebServerRequest * request) {
      if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
        return request->requestAuthentication();
      AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/javascript"), chart_js, chart_js_size);
      response->addHeader(F("Content-Encoding"), F("gzip"));
      request->send(response);
    });
  */

  server.on("/fonts/3ds.ttf", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/x-font-ttf"), font3ds_ttf, font3ds_ttf_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  /*server.on("/fonts/font.ttf", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("application/x-font-ttf"), font_ttf, font_ttf_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
    });*/

  server.on("/fonts/fonticon.woff2", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("font/woff2"), fonticon_woff2, fonticon_woff2_size);
    request->send(response);
  });

  server.on("/favicon.ico", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/x-icon"), favicon_ico, favicon_ico_size);
    request->send(response);
  });

  server.on("/icon-192x192.png", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/png"), icon_192x192, icon_192x192_size);
    response->addHeader(F("Content-Encoding"), F("gzip"));
    request->send(response);
  });

  //https://webformyself.com/neskolko-sovetov-kotorye-sdelayut-pwa-na-ios-poxozhimi-na-rodnye-prilozheniya/
  server.on("/manifest.json", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    String M = F("{\"background_color\":\"#333\",\"theme_color\":\"#333\",\"description\":\"BierLift 2\",\"display\":\"standalone\",\"start_url\":\"/\",\"name\":\"");
    M += _app_name;
    M += F("\",\"short_name\":\"");
    M += _app_name;
    M += F("\",\"icons\":[{\"src\":\"/icon-192x192.png\",\"sizes\":\"192x192\",\"type\":\"image/png\"}]}");
    request->send(200, F("application/json"), M);
  });

  server.on("/logout", HTTP_GET, [this](AsyncWebServerRequest * request) {
    if (_AuthenticateStatus && !request->authenticate(_WebAuthLogin.c_str(), _WebAuthPass.c_str()))
      return request->requestAuthentication();
    request->send(401);
  });

  server.onNotFound([this](AsyncWebServerRequest * request) {
    request->redirect("/");
  });
}


void CRMui3::setWebAuth(const String &login, const String &pass) {
  if (login != "") {
    _WebAuthLogin = login;
    _WebAuthPass = pass;
    _AuthenticateStatus = true;
  } else _AuthenticateStatus = false;
}


void CRMui3::setApiKey(const String &key) {
  if (api) _apiKey = key;
}


uint8_t CRMui3::webConnCountStatus() {
  return ws.count();
}


bool CRMui3::webConnStatus() {
  return _sendingToWeb;
}


void CRMui3::apiResponse(const String &p, const String &v) {
  _apiResponse[_apiResponse.length() - 1] = ',';
  _apiResponse += "\"" + p + "\":\"" + v + "\"}";
}

//*webUpdateBuf
void CRMui3::webUpdate(const String & name, const String & value, bool n) {
  if (_sendingToWeb) {
    if (name == "") {
      ws.textAll(F("{\"_t\":0}"));
    } else {
      if (!_webbuf.startsWith("{")) _webbuf = "{\"_t\":2,\"d\":[";
      _webbuf += "[\"" + name + "\",\"" + value + "\"],";
      uint16_t blen = _webbuf.length();
      if (n || blen > 350) {
        _webbuf[blen - 1] = ']';
        _webbuf += "}";
        ws.textAll(_webbuf.c_str());
        _webbuf = String();
      }
    }
  }
}


void CRMui3::webNotif(const String &type, const String &text, long tout, bool x) {
  if (_sendingToWeb) {
    String s(F("{\"_t\":3,\"d\":[\""));
    s += type;
    s += F("\",\"");
    s += text;
    s += F("\",");
    s += tout;
    s += ",";
    s += x;
    s += F("]}");
    ws.textAll(s.c_str());
  }
}


String CRMui3::uint64ToString(uint64_t v) {
  String s = String();
  do {
    char c = v % 10;
    v /= 10;
    if (c < 10) c += '0';
    else c += 'A' - 10;
    s = c + s;
  } while (v);
  return s;
}


void CRMui3::espReboot() {
  cfgSave();
  delay(200);
  DBGLN(F("ESP Reboot..."));
  ESP.restart();
}


void CRMui3::espSleep(uint32_t sec, bool m) {
#ifdef ESP32
  if (sec) esp_sleep_enable_timer_wakeup(sec * 1000000ULL);
  if (m) {
    SPLN(F("[SLEEP] Deep sleep started..."));
    esp_deep_sleep_start();
  } else {
    SPLN(F("[SLEEP] Light sleep started..."));
    esp_light_sleep_start();
  }
  SPLN(F("[SLEEP] Light sleep ended."));
  _upTime += sec;
  //esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_TIMER);
#else
  Serial.println(F("The code for ESP8266 not written.\nPlease view exemple esp8266/LowPowerDemo/LowPowerDemo.ino"));
#endif
}
