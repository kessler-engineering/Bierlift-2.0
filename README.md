# BierLiftOS – Firmware für den Bierlift 2.0

**BierLiftOS** ist die Steuersoftware des [Bierlift 2.0](https://www.bierlift.de/bierlift-2-0/bierlift-2-0-projektbeschreibung/), eines im Boden versenkten Getränkekühlers zum Selbstbauen. Auf Knopfdruck fährt der Lift die Getränke aus der kühlen Erde nach oben, mit Musik und Lichteffekten. BierLiftOS läuft auf einem **ESP32** auf dem Bierlift-Control-Board und wird über eine Weboberfläche im Browser bedient, ohne App-Installation.

Projektseite mit Bauanleitung, Stückliste, Konfigurator und Shop: **[www.bierlift.de](https://www.bierlift.de)**

> *English summary:* BierLiftOS is the ESP32 firmware for the DIY in-ground beer cooler "Bierlift 2.0". It drives the lift motor and provides a multilingual (DE/EN/RU) progressive web app, 433 MHz remote control, push button, WS2812B lighting, MP3 playback, water pump control, DS18B20 temperature sensors, weather-aware "Night Cooling", Blynk IoT, Telegram bot, MQTT and a simple HTTP API. Build with PlatformIO (`pio run`), see below.

**Aktuelle Version:** 2026.11.13d

---

## Inhalt

- [Funktionen im Überblick](#funktionen-im-überblick)
- [Hardware](#hardware)
- [Bedienung](#bedienung)
- [Menüs und Einstellungen](#menüs-und-einstellungen)
- [Night Cooling](#night-cooling)
- [Schnittstellen: HTTP-API, MQTT, Blynk, Telegram](#schnittstellen)
- [Installation und Update](#installation-und-update)
- [Selbst bauen (PlatformIO)](#selbst-bauen-platformio)
- [Projektstruktur](#projektstruktur)
- [Änderungen in 2026.11.12/13](#änderungen-in-2026111213)
- [Änderungen in 2026.11.11](#änderungen-in-20261111)
- [Sicherheitshinweise](#sicherheitshinweise)
- [Lizenz und Danksagung](#lizenz-und-danksagung)

---

## Funktionen im Überblick

| Bereich | Was BierLiftOS macht |
|---|---|
| **Liftsteuerung** | Hoch, Stopp, Runter, optional Schnellgang; Fahrzeiten einstellbar; Abschaltung über Bewegungssensor (Lichtschranke) in der Endlage |
| **Bedienung** | Weboberfläche (PWA) für Smartphone, Tablet und PC, Drucktaster am Lift, 433-MHz-Funkfernbedienung, Telegram, Blynk-App, MQTT, HTTP-Befehle |
| **WLAN** | Eigener Access Point „BierliftKE“ (funktioniert ohne Heimnetz), Client im Heimnetz oder beides gleichzeitig |
| **Beleuchtung** | WS2812B-LED-Streifen (bis 144 LEDs) mit Effekten oder fester Farbe, Geschwindigkeit, Helligkeit, Laufrichtung |
| **Musik** | DFPlayer-MP3-Modul mit Micro-SD-Karte, eigene Titel für Hoch- und Runterfahren, Zufallswiedergabe, Lautstärke, Equalizer |
| **Temperatur** | Zwei DS18B20-Sensoren (innen und außen), Anzeige in der Oberfläche, per API, MQTT, Blynk und Telegram |
| **Wasser** | Wassersensor und Pumpe, automatischer oder manueller Pumpbetrieb mit Nachlaufzeit |
| **Night Cooling** | Öffnet den Lift nachts automatisch, wenn es draußen kühler ist, mit Wetterprüfung (Regen, Wind) über Open-Meteo |
| **Komfort** | Kindersicherung, frei schaltbare Zusatzausgänge (GPIO 22/25/26), drei Sprachen (Deutsch, Englisch, Russisch), Farbschema, Sicherung und Wiederherstellung der Einstellungen, Firmware-Update über den Browser, Update-Hinweis bei neuer Version |

---

## Hardware

BierLiftOS ist für die **Bierlift-2.0-Steuerung** gemacht: ESP32-DevKit mit **30 Pins** (ESP32-WROOM-32, z. B. DOIT ESP32 DevKit V1) auf dem Control Board, verbunden mit dem Adapter Board, an dem Motor, Sensoren und Peripherie hängen. Eine Anleitung zur Elektrik steht auf [bierlift.de → Elektrik und Steuerung](https://www.bierlift.de/bierlift-2-0/4-elektrik-und-steuerung/). Die alte Elektronik (Bierlift-Shield / Bierlift 1.0) lässt sich mit Einschränkungen betreiben (Option unter *Verschiedenes*).

### Pinbelegung (ESP32)

| GPIO | Funktion |
|---|---|
| 2 | Blaue Onboard-LED (abschaltbar) |
| 12 | Relais „runter“ |
| 13 | Relais „hoch“ |
| 14 | Schnellgang |
| 15 | DS18B20-Temperatursensoren (1-Wire) |
| 18 / 19 | DFPlayer Mini (TX / RX) |
| 21 | MOSFET-Ausgang (reserviert) |
| 22, 25, 26 | Frei nutzbare Zusatzausgänge (Menü *Development*) |
| 23 | WS2812B-Daten (DIN) |
| 27 | Wasserpumpe |
| 32 | Bewegungssensor / Lichtschranke |
| 33 | Drucktaster |
| 34 | Wassersensor |
| 35 | 433-MHz-Funkempfänger |
| 0 | BOOT-Taste |

### Speicher

4-MB-Flash mit Partitionstabelle `min_spiffs`: zwei App-Bereiche zu je 1,9 MB (für Updates über den Browser) und 192 KB SPIFFS für die Einstellungen.

---

## Bedienung

### Erster Start

1. ESP32 mit Strom versorgen (Netzteil des Lifts oder USB-Ladegerät).
2. Am Smartphone oder PC mit dem WLAN **„BierliftKE“** verbinden (ab Werk ohne Passwort). Den Hinweis „kein Internet“ ignorieren.
3. Im Browser **http://192.168.4.1** öffnen. Die Hauptseite von BierLiftOS erscheint.
4. Unter **WLAN** den Modus **„Access Point + Client“** wählen, Name und Passwort des Heimnetzes eintragen, speichern. Der Lift startet neu und verbindet sich.
5. Die Adresse im Heimnetz steht unten in der Navigation unter **IP**. Ab dann den Lift über diese Adresse aufrufen.

Auf dem Smartphone lässt sich die Seite über „Zum Startbildschirm hinzufügen“ wie eine App ablegen.

**Hinweis WLAN:** Der ESP32 funkt nur im 2,4-GHz-Band. Ist der Funkkanal des Routers stark belegt, reagiert die Weboberfläche träge. Ein fester, weniger belegter Kanal im Router (z. B. 1 statt 6) hilft.

### Drucktaster am Lift

| Bedienung | Wirkung |
|---|---|
| 1× drücken | Hoch, Stopp oder Runter (wechselt nach jedem Stopp) |
| 2× drücken | Schnellgang (nur wenn aktiviert) |
| 3× drücken | WLAN-Modul neu starten |
| 5× drücken | ESP32 neu starten |
| 10 s halten | Werkseinstellungen |

Mit aktiver **Kindersicherung** ist der Drucktaster gesperrt.

### Funkfernbedienung (433 MHz)

Bis zu vier Tasten eines handelsüblichen 433-MHz-Handsenders lassen sich je einer Funktion zuordnen (hoch / stopp / runter im Wechsel, hoch, runter, Stopp, Musik lauter oder leiser, Beleuchtung ein oder aus, Pumpe ein oder aus, WLAN neu verbinden, ESP32 neu starten). Der zuletzt empfangene Code wird angezeigt und kann übernommen werden. Anleitung: [Fernbedienung programmieren](https://www.bierlift.de/rc_programming/).

---

## Menüs und Einstellungen

| Menü | Inhalt |
|---|---|
| **Steuerung** | Status, Temperatur innen/außen, Wasserstand, Tasten hoch / stopp / runter, optionale Schalter |
| **Beleuchtung** | Anzahl LEDs, Effekt oder feste Farbe, Richtung, Geschwindigkeit, Anzahl gleichzeitig leuchtender LEDs, Helligkeit |
| **Wasser** | Pumpe und Wassersensor aktivieren, Sensorlogik umkehren, Automatik mit Nachlaufzeit, Start/Stopp von Hand |
| **Temperatur** | Sensoren innen/außen, Sensoren tauschen, Night Cooling |
| **Musik** | Musik beim Hoch-/Runterfahren, Lautstärke, Equalizer (Normal, Pop, Rock, Jazz, Classic, Bass), Zufall, Titelnummern, Titelliste mit Wiedergabe |
| **Blynk IoT** | Blynk ein/aus, Template-ID, Auth-Token |
| **Funkfernbedienung** | Codes und Funktionen der Tasten 1–4 |
| **Telegram** | Bot ein/aus, Bot-Token, Admin-ID, Meldungen bei Statuswechsel |
| **MQTT** | Server, Port, Login, Passwort, Topic-Präfix |
| **WLAN** | Modus (Access Point, Client, beides), Heimnetz, Access-Point-Name und -Passwort, Wartezeit bis zum Rückfall in den AP-Modus |
| **Fahrt** | Fahrzeiten hoch und runter, Bewegungssensor, Schnellgang, Schnell/Normal tauschen |
| **Verschiedenes** | Alte Elektronik (Bierlift Shield / 1.0), Kindersicherung, Onboard-LED, Update-Prüfung |
| **System** | Sprache, Zeitzone, Sommerzeit automatisch (EU), Farben, Werkseinstellungen, Einstellungen sichern und wiederherstellen, Firmware-Update |
| **Development** | Zusatzausgänge GPIO 22/25/26: Grundzustand, beim Hoch- oder Runterfahren schalten, als benannter Schalter auf der Steuerseite |

Unten in der Navigation stehen Betriebszeit, WLAN-Signal, freier Speicher, Geräte-ID, IP-Adresse und Firmware-Version.

Ausführliche Beschreibung mit Bildern: [bierlift.de → Funktionen und Einstellungen](https://www.bierlift.de/bierlift-web-server/funktionen-und-einstellungen/).

---

## Night Cooling

Night Cooling nutzt kühle Nachtluft: Ist es draußen um die eingestellte Differenz (Hysterese) kühler als im Lift, fährt der Lift im eingestellten Zeitfenster ein Stück hoch (Hubzeit in Millisekunden bestimmt, wie weit). Sind die Bedingungen nicht mehr erfüllt, schließt er wieder.

- Benötigt beide Temperatursensoren und die Uhrzeit aus dem Internet (NTP), also eine Verbindung ins Heimnetz.
- **Wetter berücksichtigen:** Mit Standortkoordinaten (z. B. aus Google Maps kopiert) holt BierLiftOS alle 20 Minuten Niederschlag und Windgeschwindigkeit von [Open-Meteo](https://open-meteo.com). Bei Regen oder zu viel Wind öffnet der Lift nicht bzw. schließt.
- **Sicher bei fehlenden Daten:** Sind die Wetterdaten älter als eine Stunde, öffnet der Lift nicht und ein offener Lift schließt. Die Anzeige zeigt dann „n/a“.
- **Zeitzone:** Mit „Sommerzeit automatisch (EU)“ wird die Winterzeit eingetragen (Deutschland: 60 Minuten); die Umstellung erfolgt dann selbst.

Hintergrund: [Night Cooling auf bierlift.de](https://www.bierlift.de/night-cooling/).

---

## Schnittstellen

### HTTP-API

`http://<IP-des-Lifts>/api?...`

| Aufruf | Wirkung |
|---|---|
| `action=2` / `1` / `0` / `-1` / `-2` | Schnell hoch / hoch / Stopp / runter / schnell runter |
| `state` | Status als Text |
| `t` | Beide Temperaturen |
| `t1` / `t2` | Temperatur innen / außen |

### MQTT

`[Präfix/]` ist optional (Einstellung *Präfix*, endet mit `/`).

| Richtung | Topic | Inhalt |
|---|---|---|
| empfangen | `[Präfix/]BierLift/a` | 2 = schnell hoch, 1 = hoch, 0 = Stopp, -1 = runter, -2 = schnell runter |
| empfangen | `[Präfix/]BierLift/t` | leere Nachricht → Temperaturen werden gesendet |
| empfangen | `[Präfix/]BierLift/s` | leere Nachricht → Status wird gesendet |
| empfangen | `[Präfix/]BierLift/chprot` | Kindersicherung 1/0 |
| empfangen | `[Präfix/]BierLift/gpio22`, `gpio25`, `gpio26` | Zusatzausgang 1/0 (wenn aktiviert) |
| senden (retained) | `[Präfix/]BierLift/s` | Status- und Ereignismeldungen |
| senden (retained) | `[Präfix/]BierLift/t/in`, `t/out` | Temperatur innen / außen in °C |
| senden (retained) | `[Präfix/]BierLift/w` | Wasserstand 1 = hoch, 0 = niedrig |

Gespeicherte (retained) Befehle werden bewusst ignoriert, damit der Lift nach einem Neustart nicht von selbst losfährt.

### Blynk IoT

Steuerung über die Blynk-App und die Blynk-Cloud, auch von unterwegs und per Sprachassistent. Virtuelle Pins: **V0** Fahrt (1 = hoch, 0 = Stopp, -1 = runter), **V1** Temperatur innen, **V4** Temperatur außen.

### Telegram

Eigener Telegram-Bot (Token von @BotFather) zum Steuern und für Meldungen an den Admin, etwa wenn jemand den Lift benutzt.

---

## Installation und Update

### Update über den Browser (empfohlen)

1. Neue Firmware-Datei (`.bin`) aus den [Releases](https://github.com/kessler-engineering/Bierlift-2.0/releases) herunterladen.
2. Vorher unter **System → Einstellungen sichern** eine Sicherung speichern.
3. **System → Firmware-Update** öffnen, die Datei auswählen, hochladen. Der Lift darf dabei nicht fahren.
4. Nach dem Neustart prüfen, ob die neue Version unten in der Navigation steht.

Seit 2026.11.11 enthält die Sicherungsdatei **keine Passwörter und Tokens** mehr. Nach dem Wiederherstellen auf einem neuen ESP32 müssen WLAN-Passwort, Telegram-Token, MQTT-Passwort und Blynk-Token neu eingegeben werden.

BierLiftOS prüft, ob es auf GitHub ein neueres Release gibt, und zeigt dann einen Hinweis (abschaltbar unter *Verschiedenes*). Die erste Prüfung läuft direkt nach dem Verbinden mit dem WLAN; Telegram und Blynk starten erst danach (höchstens 90 s Wartezeit). Danach wird alle 12 Stunden erneut geprüft, aber nur, wenn genug Speicher frei ist (60 KB am Stück), sonst wird die Prüfung übersprungen. Mit Telegram und Blynk erscheint der Hinweis deshalb in der Regel erst nach einem Neustart.

### Erstinstallation auf einem leeren ESP32

Gekaufte Steuerungen aus dem [Bierlift-Shop](https://www.bierlift.de/shop/) sind schon fertig bespielt. Für einen eigenen ESP32 die Dateien aus [`BierLift2/_bin`](BierLift2/_bin) mit dem *Flash Download Tool* von Espressif oder mit `esptool` schreiben (SPI-Modus DOUT, 40 MHz):

| Adresse | Datei |
|---|---|
| `0x1000` | `bootloader_dout_40m_0x1000.bin` |
| `0x8000` | `partitions_0x8000.bin` |
| `0xE000` | `boot_0xe000.bin` |
| `0x10000` | `BierLiftOS_v2026.11.13d_0x10000.bin` |

```bash
esptool.py --chip esp32 --baud 921600 write_flash --flash_mode dout --flash_freq 40m \
  0x1000 bootloader_dout_40m_0x1000.bin 0x8000 partitions_0x8000.bin \
  0xe000 boot_0xe000.bin 0x10000 BierLiftOS_v2026.11.13d_0x10000.bin
```

Vorher den Flash löschen (`esptool.py erase_flash`). Bebilderte Anleitung: [bierlift.de → Installation](https://www.bierlift.de/bierlift-web-server/installation/).

---

## Selbst bauen (PlatformIO)

Voraussetzung: [PlatformIO](https://platformio.org) (VS Code-Erweiterung oder PlatformIO Core).

```bash
git clone https://github.com/kessler-engineering/Bierlift-2.0.git
cd Bierlift-2.0
pio run                    # bauen → .pio/build/bierlift/firmware.bin
pio run -t upload          # per USB aufspielen
pio device monitor         # serielle Ausgabe, 115200 Baud
```

`firmware.bin` ist die Datei für das Update über den Browser.

Wichtig für einen funktionierenden Build:

- **Plattform `espressif32@3.5.0`** = Arduino-ESP32-Core **1.0.6**. Neuere Cores (2.x/3.x) sind nicht kompatibel.
- **Partitionstabelle `min_spiffs` nicht ändern**, sonst funktionieren Updates über den Browser und gespeicherte Einstellungen nicht mehr.
- Die Bibliotheken in [`lib/`](lib) (AsyncTCP mit größerem Verbindungs-Backlog, ESPAsyncWebServer, AsyncMqttClient, Blynk 1.0.1 mit `setTemplateID`-Erweiterung) sind bewusst eingefroren, weil passende Versionen in der Registry nicht mehr verfügbar sind. Details in [`lib/README.md`](lib/README.md).
- **Weboberfläche:** Skript und Stil stehen in `BierLift2/web_script.js.h` (aktive, verschleierte Fassung oben; die Texte der WLAN- und Systemseite im Array `lang`) und `BierLift2/web_style.css.h`. Ausgeliefert wird die gzip-Fassung `BierLift2/web_gz.h`, die `tools/web_gzip.py` beim Bauen mit PlatformIO automatisch erzeugt. `web_gz.h` nie von Hand ändern.
- Alle übrigen Bibliotheken sind in `platformio.ini` mit fester Version eingetragen (ArduinoJson 6.21.5, FastLED 3.5.0, DFRobotDFPlayerMini 1.0.6, rc-switch 2.6.4, OneWire 2.3.7, DallasTemperature 3.11.0, FastBot 2.27.3).

Der Quellcode in `BierLift2/` lässt sich weiterhin auch mit der Arduino-IDE öffnen (Board „ESP32 Dev Module“, Core 1.0.6, Partition „Minimal SPIFFS“). Wurden `web_script.js.h` oder `web_style.css.h` geändert, vorher `python tools/web_gzip.py` ausführen, damit `web_gz.h` aktuell ist.

---

## Projektstruktur

```
platformio.ini          PlatformIO-Projekt (src_dir = BierLift2)
lib/                    eingefrorene Bibliotheken
tools/web_gzip.py       erzeugt beim Bauen BierLift2/web_gz.h (gzip von Skript und Stil)
tools/font_icons.py     baut die Steuer-Symbole in die Symbol-Schrift web/fonticon.woff2.h ein (fontTools, brotli)
BierLift2/
├── BierLift2.ino       Start, setup() und loop()
├── 00_sett.h           Pins, globale Einstellungen
├── 00_lng.ino          Oberflächentexte DE / EN / RU
├── 01_interface.ino    Aufbau der Weboberfläche (Menüs, Felder)
├── 02_api.ino          HTTP-API
├── 03_update.ino       Reaktion auf geänderte Einstellungen
├── 04_butt.ino         Drucktaster
├── 05_blynk.ino        Blynk IoT
├── 06_led.ino          WS2812B-Beleuchtung
├── 07_mp3.ino          DFPlayer / Musik
├── 08_pump.ino         Wasserpumpe und Wassersensor
├── 09_rele.ino         Motorsteuerung, Befehlswarteschlange
├── 10_rf.ino           433-MHz-Fernbedienung
├── 11_sensor.ino       Temperatur, Night Cooling
├── 12_tg.ino           Telegram-Bot
├── 13_dev.ino          Zusatzausgänge (Development)
├── 14_MQTT.ino         MQTT
├── 15_metric.ino       Update-Prüfung über GitHub-Releases
├── 16_time.ino         Uhrzeit (NTP), Zeitzone, Sommerzeit
├── 17_weather.ino      Wetter über Open-Meteo
├── CRMui3.*, ui.cpp,   Web-Framework CRMui3 (Oberfläche, Konfiguration,
│   config.cpp, var.cpp, WLAN, GPIO)
│   wi-fi.cpp, gpio.cpp
├── web_*.h, web/       Weboberfläche (HTML, CSS, JS, Schriften, Icons)
├── web_gz.h            gzip-Fassung von Skript und Stil (automatisch erzeugt)
├── _bin/               fertige Binärdateien für die Erstinstallation
└── _docs/              ältere Projektunterlagen
```

---

## Änderungen in 2026.11.12/13

Version 2026.11.13d behebt vor allem Netzwerk- und Absturzfehler, die in 2026.11.11 noch enthalten waren.

**Stabilität und Netzwerk**
- **Netzwerkfehler behoben:** Die Hauptschleife fragte die Befehlswarteschlange bei jedem Durchlauf ab (über 100 000-mal pro Sekunde, jedes Mal mit kurzer Interrupt-Sperre) und gab nie Rechenzeit ab. Folge waren bis zu 93 % Paketverlust und eine Weboberfläche, die nicht lud. Jetzt wird die Warteschlange nur bei Bedarf abgefragt, und die Hauptschleife legt pro Durchlauf eine Pause von 1 ms ein.
- **WLAN-Stromsparmodus aus:** kürzere Antwortzeiten im Client-Modus.
- **Absturz bei der Update-Prüfung behoben:** Neben Telegram und Blynk reichte der Speicher nicht für eine dritte verschlüsselte Verbindung; der ESP startete dann alle 2 Minuten neu. Jetzt läuft die Prüfung beim Start vor Telegram und Blynk und danach nur bei genug freiem Speicher. Der DNS-Server ist zusätzlich abgesichert.
- **WLAN-Wiederverbindung:** Im Modus „Access Point + Client“ gab die Firmware nach einem Verbindungsverlust endgültig auf. Jetzt versucht sie es alle 15 s erneut und protokolliert den Trennungsgrund. Getestet: WLAN aus und wieder an, der ESP verbindet sich von selbst.

**Weboberfläche**
- **Übertragungsfehler bei `/ui` behoben:** Ab 7000 Byte wurde aus einem bereits freigegebenen Puffer gesendet; die Seite stockte oder brach ab (Fehler aus dem ursprünglichen CRMui3).
- **Browser-Cache:** Skript, Stil und die übrigen Dateien der Oberfläche haben versionierte Adressen und bleiben im Browser gespeichert; die Startseite wird per `ETag` geprüft. Beim Öffnen werden nur noch die Startseite und die Daten übertragen.
- **gzip:** `script.js` 30,6 → 12,4 KB, `style.css` 20,7 → 4,5 KB, Firmware 34 KB kleiner.
- **Mehr gleichzeitige Verbindungen:** AsyncTCP nimmt bis zu 12 statt 5 wartende Verbindungen an.
- **Neue Steuer-Symbole:** schlichte Winkel für hoch/runter, Doppelwinkel für den Schnellgang und ein abgerundetes Quadrat für Stopp (nach den Tabler-Icons, MIT-Lizenz). Alle Tasten sind gleich groß und haben gleiche Abstände.

**Sound**
- **MP3-Player ohne Bestätigungsmodus:** Die Bibliothek konnte darin endlos warten, der Start blieb stehen.
- **Titelanzahl** wird jetzt richtig gelesen.
- **Abstand zwischen Befehlen:** mindestens 120 ms zwischen zwei Befehlen an den Player; vorher startete die Musik manchmal erst nach mehreren Klicks.

**Sonstiges**
- Die serielle Ausgabe der Konfiguration zeigt Passwörter und Tokens nur noch als `********`.

**Messwerte** (13.11.2026, zwei Steuerungen): mit Telegram, Blynk, Temperatur und Night Cooling 0 % Paketverlust, Ping im Median 8 ms, Weboberfläche in 0,10 s; mit MP3-Player 0 % Paketverlust, 5 ms, 0,12 s, Musik startet beim ersten Klick.

## Änderungen in 2026.11.11

Gegenüber der bisher veröffentlichten Version (Januar 2024) wurde die Firmware gründlich überarbeitet:

**Sicherheit**
- WLAN-Passwort, Access-Point-Passwort, Telegram-Token, MQTT-Passwort und Blynk-Token werden in der Oberfläche und in der Sicherungsdatei nur noch als `********` angezeigt. Vorher konnte jeder im offenen Access Point das Heim-WLAN-Passwort auslesen.
- ArduinoOTA ist standardmäßig aus; der Firmware-Upload prüft die Web-Anmeldung (falls aktiviert).
- Absturz der API bei fehlenden Parametern behoben; fest eingebautes Blynk-Token entfernt.

**Stabilität**
- Alle Fahrbefehle (Web, API, MQTT, Blynk, Telegram, Funk, Night Cooling) laufen über eine Befehlswarteschlange und werden nur in der Hauptschleife ausgeführt.
- Keine blockierenden Aufrufe mehr in der Hauptschleife (Telegram, Wetter, Blynk und Update-Prüfung in eigenen Tasks).
- Mögliche Blockade beim Beenden von Tasks behoben, MQTT-Speicherleck behoben, LED-Anzahl auf 1–144 begrenzt.

**Funktionen**
- Update-Prüfung über GitHub-Releases, Versionsvergleich repariert.
- Sommerzeit automatisch (EU) als neue Option.
- Wetter für Night Cooling über Open-Meteo (der bisherige Dienst lieferte keine Daten mehr), sicheres Verhalten bei fehlenden Wetterdaten.

**Projekt**
- PlatformIO-Projekt mit festen Bibliotheksversionen.
- Kommentare im Code durchgehend auf Deutsch, Oberflächentexte in DE/EN/RU überarbeitet und vereinheitlicht.
- 2026.11.11c: Texte der WLAN- und Systemseite korrigiert (z. B. „Max. wait for connection“, „Passwort“, „Name des Access Points“).

---

## Sicherheitshinweise

- Der Bierlift arbeitet mit Netzspannung (Netzteil) und einem Motor mit Quetschgefahr. Arbeiten an der Elektrik nur stromlos und im Zweifel durch eine Fachkraft; ein FI-Schutzschalter wird empfohlen.
- Den Access Point „BierliftKE“ unter *WLAN* mit einem Passwort absichern, wenn der Lift in Reichweite fremder Personen steht.
- Die Kindersicherung sperrt den Drucktaster am Lift.

---

## Lizenz und Danksagung

Das Projekt Bierlift steht laut [bierlift.de](https://www.bierlift.de) unter **Creative Commons Namensnennung – Nicht kommerziell 4.0 International (CC BY-NC 4.0)**. Die Bibliotheken in `lib/` behalten ihre eigenen Lizenzen (LGPL bzw. MIT, siehe die jeweiligen Ordner).

Die Weboberfläche basiert auf dem Framework **CRMui3**. Weitere verwendete Bibliotheken: ArduinoJson, FastLED, DFRobotDFPlayerMini, rc-switch, OneWire, DallasTemperature, FastBot, AsyncTCP, ESPAsyncWebServer, AsyncMqttClient und Blynk. Wetterdaten: [Open-Meteo](https://open-meteo.com) (CC BY 4.0).

**kessler engineering** · [www.bierlift.de](https://www.bierlift.de) · Fragen und Hinweise gern über das [FAQ](https://www.bierlift.de/faq/) oder als GitHub-Issue.
