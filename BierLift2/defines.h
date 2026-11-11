#define CRM_VER "2026.11.11b"

#define CONFIG_BUTTON_PIN 0
#define AUTO_SAVE_TIME_INTERVAL 30000

// Platzhalter für Passwörter/Tokens im Web-Interface und in der Backup-Datei
#define SECRET_MASK "********"

// ArduinoOTA (Flashen über die Arduino-IDE im Netzwerk). Für die Auslieferung aus lassen:
// ohne Passwort könnte jeder im Netz eine fremde Firmware aufspielen.
// Updates für Kunden laufen weiter über das Web-Interface ("Firmware update").
// Steht hier (nicht in 00_sett.h), weil auch CRMui3.cpp die Werte sehen muss.
//#define USE_ARDUINO_OTA
//#define ARDUINO_OTA_PASSWORD "bitte-aendern"

#define INPUT_BUTTON 0
#define INPUT_CHECKBOX 1
#define INPUT_PASSWORD 5
#define INPUT_TEXT 9
#define INPUT_COLOR 10
#define INPUT_DATE 11
#define INPUT_DATETIME 12
#define INPUT_EMAIL 13
#define INPUT_NUMBER 14
#define INPUT_RANGE 15
#define INPUT_SELECT 17
#define INPUT_TIME 18
#define INPUT_WIFI 19

#define CARD_CHECKBOX 20
#define CARD_TEXT 21
#define CARD_BUTTON 22
#define CARD_CHART_L 23
#define CARD_CHART_B 24

#define OUTPUT_TEXT 30
#define OUTPUT_TABL 31
#define OUTPUT_HR 32
#define CHART_L 33
#define CHART_B 34

#define DBG if (_debug) Serial.print
#define DBGLN if (_debug) Serial.println
#define SPLN Serial.println
#define SP Serial.print

#define TRACE() \
  Serial.print(__FILE__); \
  Serial.print(" | "); \
  Serial.print(__LINE__); \
  Serial.print(" | "); \
  Serial.println(__PRETTY_FUNCTION__); \
  Serial.flush();
  
