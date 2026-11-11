# Eingefrorene Bibliotheken für BierLiftOS (Core 1.0.6)

Diese drei Bibliotheken liegen fest im Projekt, weil die passenden alten Versionen
in der PlatformIO-Registry nicht mehr verfügbar sind (die me-no-dev-Repos wurden
im Januar 2025 archiviert, die Registry-Einträge zeigen jetzt auf die neue
3.x-Linie, die Core 1.0.6 nicht mehr unterstützt).

| Ordner            | Quelle                                         | Stand              |
|-------------------|------------------------------------------------|--------------------|
| AsyncTCP          | github.com/me-no-dev/AsyncTCP                  | ca8ac5f (v1.1.1)   |
| ESPAsyncWebServer | github.com/me-no-dev/ESPAsyncWebServer         | f71e3d4 (v1.2.3)   |
| AsyncMqttClient   | github.com/marvinroger/async-mqtt-client       | v0.9.0             |
| Blynk             | github.com/blynkkk/blynk-library               | v1.0.1 + Anpassung |

In `library.json` wurden nur die `dependencies` entfernt, damit PlatformIO nicht
die neuen 3.x-Versionen aus der Registry nachlädt. Der Code von AsyncTCP,
ESPAsyncWebServer und AsyncMqttClient ist unverändert.

**Blynk:** `setTemplateID()` gibt es in keiner offiziellen Blynk-Version – der
bisherige Build lief mit einer lokal angepassten Bibliothek. Die Anpassung ist hier
nachgebaut (alle Stellen mit „BierLiftOS-Anpassung“ markiert):
- `src/Blynk/BlynkApi.h`: Methode `setTemplateID(const char*)` und Puffer `_tmplId`
- `src/BlynkApiArduino.h`: `sendInfo()` schickt `tmpl` aus `_tmplId`
Ab Blynk 1.2/1.3 ist das Konzept anders (BLYNK_TEMPLATE_ID/NAME fest im Code) –
bei einem Blynk-Update muss die Anpassung neu gemacht werden.
Beim späteren Umstieg auf Core 3.x diesen Ordner löschen und die Bibliotheken
von ESP32Async über `lib_deps` einbinden.
