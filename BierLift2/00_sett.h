//*********************** D E F I N I T I O N E N ***********************
#define BL_NAME "BierLift"

#define DBGRUN                            // Debug-Befehle über den seriellen Monitor (dbgRun)
// ArduinoOTA ein/aus und OTA-Passwort: siehe defines.h (USE_ARDUINO_OTA)

// Update-Prüfung: öffentliches GitHub-Repo, dessen neuestes Release (Tag z. B. "v2025.11.11r")
// als aktuelle Version gilt. Tag-Format: JJJJ.MM.TT + Buchstabe, optional mit "v" davor.
#define FW_UPDATE_REPO "kessler-engineering/Bierlift-2.0"

#define EB_DEB 80                         // Entprellzeit Drucktaster, ms
#define EB_HOLD 10000                     // Haltezeit Drucktaster für Werkseinstellungen, ms

#define MAX_LED_IN_STRIP 144

#define COMMAND_PROT_TIME 2000            // Pause zwischen zwei Relais-Umschaltungen (Richtungswechsel), ms
#define WAIT_FOR_POWER_SENSOR 100         // Verzögerung bis zur Auswertung des Bewegungssensors (Motorstrom), ms

#define DS_READ_INTERVAL 10000            // Abfrageintervall Temperatursensoren, ms

#define MAX_POWER_SUPPLY_CURRENT 4800     // Max. Strom des 5-V-Netzteils für den LED-Streifen (FastLED begrenzt die Helligkeit), mA

#define NUMBER_SOUNDTRACKS 20             // Anzahl Titel, falls der MP3-Player keine Anzahl meldet

#define SAVE_LIFT_STATUS true             // Liftposition (oben/unten) in der Konfiguration speichern

#define BLYNK_TEMPLATE_ID __DATE__        // Blynk IoT: Build-Datum als Firmware-Typ; die Template-ID kommt zur Laufzeit (setTemplateID)
#define BLYNK_PRINT Serial                // Blynk-Debugausgaben auf Serial
//#define BLYNK_DEBUG_ALL
/*#define BLYNK_HEARTBEAT 20                // Heartbeat-Intervall in Sekunden (Standard 10)
  //#define BLYNK_TIMEOUT_MS 5000UL         // Netzwerk-Timeout in ms (Standard 3000)
  //#define BLYNK_SEND_ATOMIC               // Befehl und Header in einem Paket senden (braucht mehr RAM)
  #define BLYNK_MSG_LIMIT 60                // Max. ausgehende Befehle pro Sekunde (Standard 15)
  #define BLYNK_MAX_READBYTES 1024          // Max. Länge eingehender Befehle (Standard 256)
  #define BLYNK_MAX_SENDBYTES  512          // Max. Länge ausgehender Befehle (Standard 128)*/

#define PWM_FREQ 5000                     // PWM-Frequenz für den Sanftanlauf der alten Elektronik (Bierlift-Shield), Hz

#define configUSE_IDLE_HOOK 1
#define configUSE_TICK_HOOK 1
#define configCHECK_FOR_STACK_OVERFLOW 1

//*********************** V A R I A B L E N ***********************
SemaphoreHandle_t wifi_mutex;

// Motorbefehle aus anderen Tasks (Web/API, MQTT, Blynk, Telegram, Funk, Ticker)
// landen in dieser Queue und werden nur in loop() ausgeführt.
#define LIFT_CMD_TOGGLE 100               // hoch/runter/stopp im Wechsel (wie Drucktaster)
QueueHandle_t liftCmdQueue = NULL;
TaskHandle_t loopTask_h = NULL;

// Telegram-Meldungen werden gepuffert und im Telegram-Task gesendet (kein HTTPS in loop()).
QueueHandle_t tgQueue = NULL;

// Blynk V0 wird im Blynk-Task geschrieben, loop() setzt nur den Wert.
#define BLYNK_V0_NONE 127
volatile int blynkV0Pending = BLYNK_V0_NONE;

TaskHandle_t weather_task = NULL;

TaskHandle_t task_pump_handle = NULL;
TaskHandle_t task_waterSensRead = NULL;
TaskHandle_t ds18_read_handle = NULL;
TaskHandle_t blynk_start_task = NULL;
TaskHandle_t rfControl_task = NULL;
TaskHandle_t telegram_task = NULL;
TaskHandle_t ledFlash_task = NULL;
TaskHandle_t metric_task = NULL;


uint8_t LNG;
long lastVER;
uint32_t stateTimer = 0;                                        //Allgemein
bool OLD = false, HGSP = false, CHP = false;                    //_

uint32_t releyOffTimer = 0, releyCommandProtTimer = 0,          //Relais / Motor
         UTIME = 0, DTIME = 0;                                  //
bool releyStatus = false, releyCommandStatus = false,           //
     releyDirection = true, motorSpeed = false,                 //
     lift_position = false, MOP = false, SINV = false;          //
int lastAction = 0;                                             //_

float Tin = 0, Tout = 0;                                        //Temperatur
bool VTIN = false, VTOUT = false, TSWAP = false;

bool waterVol = 0;                                              //Wasserstandsensor

bool pumpStatus = false, PUMPM = false, USEPM = false,          //Pumpe
     PUMPSENSINV = false, USEWS = false;
uint32_t DELAYPM = 0;                                           //_

uint32_t ledTimer = 0, brTimer = 0;                             //LED
bool ledAnimateStatus = false, brightnessStatus = false, INV = false;  //
byte C1 = 0, C2 = 0, C3 = 0, mainBR = 0, LEDN = 0, PARAM = 0,   //
     mLED = 0, SPEED = 0;                                       //
int curBR = 0;                                                  //_
bool LFLASH = false;

bool playStatus = false, SUSE = false, dfInit = false,          //MP3-Player
     SRND = false;
byte SVOL = 0, EQ = 0;                                          //
int fileCounts = 0;                                             //_

bool BUSE = false, blynkInitSt = false, blynkConnSt = false,    //Blynk
     blynkActionStatus = false;                                 //
int blynkAction = 0;                                            //_
bool blynk_init = true;

bool TGUSE = false, TGNOTIF = false;                            //Telegram

uint16_t pwm_vol = 0;                                           //PWM für die alte Elektronik (Bierlift-Shield)

uint32_t optoTimer = 0;                                         //Optokoppler (Bewegungssensor)
bool optoStatus = false;

uint32_t lastReceivedCode = 0, CODE1 = 0, CODE2 = 0,
         CODE3 = 0, CODE4 = 0;                                  //433-MHz-Fernbedienung
bool USERF = false;

bool CHUDT = false;                                             //Update-Prüfung

bool DP22 = false, DP22U = false, DP22NO = false, DP22D = false, DP22C = false, DP22S = false, //Development-Pins
     DP25 = false, DP25U = false, DP25NO = false, DP25D = false, DP25C = false, DP25S = false,
     DP26 = false, DP26U = false, DP26NO = false, DP26D = false, DP26C = false, DP26S = false;

bool MQUSE = false;


// Uhrzeit und Datum
struct tm TD;
int TZ;
bool DSTEU = false;                       // Sommerzeit automatisch (EU-Regel), TZ = Winterzeit

//Night Cooling (Nachtkühlung)
bool NCOOL, effMute, nCoolStatus, WTHR;
int TGIST;
uint16_t TOPN, TCSE, TMOVE;
float WSPD;
struct {
  float W;          // Wind, m/s
  bool R;           // Niederschlag ja/nein
  bool ok;          // gültige Daten empfangen
  uint32_t t;       // millis() des letzten gültigen Abrufs
} weather;

//*********************** P I N S ***********************
#define LED_ONBOARD_PIN 2
#define UP_PIN 13
#define DOWN_PIN 12
#define SEED_PIN 14
#define DS18_PIN 15
#define DFPlayerTX_PIN 18
#define DFPlayerRX_PIN 19
#define MOSFET_PIN 21
#define DEV_PIN_22 22
#define LED_DIN_PIN 23
#define DEV_PIN_25 25
#define DEV_PIN_26 26
#define PUMP_PIN 27
#define OPTO_SENSOR_PIN 32
#define BUTTON_PIN 33
#define WATER_SENS_PIN 34
#define RF_PIN 35


//Vermutlich noch frei: 36, 39, 4, 16?, 17?, 5, 3?, 1?
