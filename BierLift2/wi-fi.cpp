#include "CRMui3.h"


void CRMui3::wifiEvent() {
  static bool firstConnect = false;

  WiFi.onEvent([this](system_event_id_t event, system_event_info_t info) {
    const int r = info.disconnected.reason;
    switch (event) {
      case 5: // STA_DISCONNECTED
        connState = false;
        if (r == 2 || r == 7 || r == 15) WiFi.reconnect();
        else if ((_wifiMode == 1 || _wifiMode == 3) && !firstConnect &&
                 millis() - _connectingTimer >= _waitTimeForConnection) {
          connState = true;
          WiFi.disconnect();
          SPLN(String() + F("[WiFi] Connecting to WiFi failed. Code ") + String(r));
        }
        break;

      case 7: // STA_GOT_IP
        connState = true;
        SPLN(String() + F("[WiFi] Connecting to \"") + WiFi.SSID() + F("\" is done. ") +
             F("(IP: ") + WiFi.localIP().toString() + F(")"));
        if (r != 200 && _wifiMode == 1 && !firstConnect) {
          firstConnect = true;
          WiFi.mode(WIFI_STA);
          SPLN(F("[WiFi] AP mode disable."));
        }
        break;
    }
  });
}


void CRMui3::wifiSTA() {
  WiFi.begin(var(F("_s")).c_str(), var(F("_p")).c_str());
}


void CRMui3::wifiAP() {
  WiFi.softAP(var(F("_as")).c_str(), var(F("_ap")).c_str());
  SPLN(String(F("[WiFi] AP name: ")) + var(F("_as")) +
       F("\t(IP: ") + WiFi.softAPIP().toString() + F(")"));
}


void CRMui3::wifiStart() {
  _wifiMode = var(F("_wm")).toInt();
  _waitTimeForConnection = var(F("wtc")).toInt() * 1000UL;

  WiFi.persistent(false);
  wifiEvent();

  wifiAP();
  if (_wifiMode == 1) wifiSTA();
  else if (_wifiMode == 3) {
    WiFi.mode(WIFI_AP_STA);
    wifiSTA();
  }
  WiFi.scanNetworks(true, false, false, 150);
  _connectingTimer = millis();
}


String CRMui3::wifiScan() {
  int n = WiFi.scanComplete();
  if (n > 0) {
    String s = "{\"s\":";
    s += String(n) + ",\"n\":[";
    for (int i = 0; i < n; ++i) {
      if (i)s += ",";
      s += "[\"";
      s += WiFi.SSID(i) + "\",";
      s += String(WiFi.channel(i)) + ",";
      s += String(WiFi.RSSI(i)) + ",";
      s += String(WiFi.encryptionType(i)) + "]";
    }
    s += "]}";
    WiFi.scanNetworks(true, false, false, 150);
    return s;
  } else if (n == 0) {
    return F("{\"s\":0}");
  } else if (n == -1) {
    return F("{\"s\":-1}");
  } else if (n == -2) {
    WiFi.scanNetworks(true, false, false, 150);
    return F("{\"s\":-2}");
  }
}
