#include "CRMui3.h"
#include <esp_wifi.h>   // esp_wifi_connect() u. a. für wifiCheck()


// Früher: Der Merker "schon einmal verbunden" wurde nur im Modus "Client" gesetzt. Im Modus
// "Access Point + Client" führte deshalb JEDER spätere Verbindungsverlust (z. B. Router-Neustart)
// nach Ablauf der Wartezeit zum endgültigen Aufgeben – der ESP kam bis zum nächsten Neustart
// nicht mehr ins Heimnetz. Außerdem wurde nur bei den Fehlercodes 2/7/15 neu verbunden; nach
// schnellen Neustarts lehnte der Router die Anmeldung teils mit anderen Codes ab und der ESP
// blieb ohne WLAN (12.11.2026). Jetzt übernimmt wifiCheck() das Wiederverbinden.
void CRMui3::wifiEvent() {
  WiFi.onEvent([this](system_event_id_t event, system_event_info_t info) {
    const int r = info.disconnected.reason;
    switch (event) {
      case 5: // WLAN getrennt (STA_DISCONNECTED)
        connState = false;
        if (_staHasIP) SPLN(String() + F("[WiFi] Disconnected, reason ") + String(r));
        _staHasIP = false;
        if (r == 2 || r == 7 || r == 15) WiFi.reconnect();
        else if ((_wifiMode == 1 || _wifiMode == 3) && !_staEverConnected && !_staGaveUp &&
                 millis() - _connectingTimer >= _waitTimeForConnection) {
          // Seit dem Start nie verbunden und Wartezeit abgelaufen (z. B. falsches Passwort):
          // Access Point bleibt für die Einrichtung, neue Versuche nur noch alle 2 min.
          connState = true;
          _staGaveUp = true;
          WiFi.disconnect();
          SPLN(String() + F("[WiFi] Connecting to WiFi failed. Code ") + String(r));
        }
        break;

      case 8: // IP-Adresse verloren (STA_LOST_IP)
        _staHasIP = false;
        SPLN(F("[WiFi] IP lost"));
        break;

      case 7: // IP-Adresse erhalten (STA_GOT_IP)
        connState = true;
        _staGaveUp = false;
        _staHasIP = true;
        SPLN(String() + F("[WiFi] Connecting to \"") + WiFi.SSID() + F("\" is done. ") +
             F("(IP: ") + WiFi.localIP().toString() + F(")"));
        if (_wifiMode == 1 && !_staEverConnected) {
          WiFi.mode(WIFI_STA);
          SPLN(F("[WiFi] AP mode disable."));
        }
        _staEverConnected = true;
        break;
    }
  });
}


// Aus run(), einmal pro Sekunde: Ist das Heimnetz eingerichtet (Modus Client oder
// Access Point + Client) und besteht keine Verbindung, regelmäßig neu verbinden –
// alle 15 s, nach dem Aufgeben (nie verbunden, Wartezeit abgelaufen) alle 2 min.
//
// Wichtig (2026.11.12g): NICHT WiFi.status() verwenden. Trennt der Router mit Grund 2
// ("Anmeldung abgelaufen", z. B. beim Kanalwechsel der FRITZ!Box), lässt der Core 1.0.6 den
// Status auf WL_CONNECTED stehen. 12e/12f hielten den ESP deshalb für verbunden und versuchten
// nichts; auch WiFi.begin() kehrt in diesem Zustand sofort zurück. Eugen musste nach dem
// Kanalwechsel per EN-Taste neu starten (12.11.2026). Jetzt eigener Merker _staHasIP
// (nur durch die Ereignisse "IP erhalten/getrennt/IP verloren" gesetzt) und Neuversuch direkt
// über ESP-IDF: sauber trennen, dann esp_wifi_connect().
void CRMui3::wifiCheck() {
  if (!(_wifiMode == 1 || _wifiMode == 3) || _staHasIP) {
    _staRetryTimer = millis();
    return;
  }
  if (millis() - _staRetryTimer < (_staGaveUp ? 120000UL : 15000UL)) return;
  _staRetryTimer = millis();
  SPLN(F("[WiFi] Not connected - retrying..."));
  if (!(WiFi.getMode() & WIFI_MODE_STA)) WiFi.enableSTA(true);
  wifi_config_t conf;
  esp_wifi_get_config(WIFI_IF_STA, &conf);
  if (conf.sta.ssid[0] == 0) {
    wifiSTA();               // noch keine Zugangsdaten gesetzt -> WiFi.begin()
    return;
  }
  esp_wifi_disconnect();     // laufenden/hängenden Versuch beenden (Zugangsdaten bleiben erhalten)
  esp_wifi_connect();        // mit gespeicherten Zugangsdaten neu verbinden, alle Kanäle absuchen
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
  // WLAN-Stromsparmodus (Modem-Sleep) aus: Der ESP32 schaltet den Empfänger sonst zwischen
  // den Beacons ab -> Ping 100–300 ms, Paketverluste, träges Web-Interface. Der Lift hängt
  // am Netzteil, der Mehrverbrauch (ca. 80 mA) spielt keine Rolle.
  WiFi.setSleep(false);
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
  return F("{\"s\":-1}");
}
