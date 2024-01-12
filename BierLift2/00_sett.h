//*********************** D E F I N E ***********************
#define BL_NAME "BierLift"

#define DBGRUN                            // терминал дебаг

#define EB_DEB 80                         // антидребезг кнопки, мс
#define EB_HOLD 10000                     // таймаут долгово удержания кнопки, мс

#define MAX_LED_IN_STRIP 144

#define COMMAND_PROT_TIME 2000            // Задержка между переключениями реле, мс
#define WAIT_FOR_POWER_SENSOR 100         // Задержка проверки датчика мотора, мс

#define DS_READ_INTERVAL 10000            // Опрос датчика температуры, мс

#define MAX_POWER_SUPPLY_CURRENT 4800     // Максимальный ток блока питания, мА

#define NUMBER_SOUNDTRACKS 20             // Максимальное количество треков, шт

#define SAVE_LIFT_STATUS true             // Сохраняет положение лифта в конфиг и во флеш

#define BLYNK_TEMPLATE_ID __DATE__        // BlynkIO (New ver)
#define BLYNK_PRINT Serial                // Blynk debag
//#define BLYNK_DEBUG_ALL
/*#define BLYNK_HEARTBEAT 20                // Heartbeat period in seconds. (default 10)  20
  //#define BLYNK_TIMEOUT_MS 5000UL         // Network timeout in milliseconds. (default 3000UL)  2000
  //#define BLYNK_SEND_ATOMIC               // Uncomment to append command body to header (uses more RAM)
  #define BLYNK_MSG_LIMIT 60                // Limit the amount of outgoing commands per second. (def 15)
  #define BLYNK_MAX_READBYTES 1024          // Limit the incoming command length. (def 256)
  #define BLYNK_MAX_SENDBYTES  512          // Limit the outgoing command length. (def 128)*/

#define PWM_FREQ 5000                     // Частота ШИМ, Гц

#define configUSE_IDLE_HOOK 1
#define configUSE_TICK_HOOK 1
#define configCHECK_FOR_STACK_OVERFLOW 1

//*********************** V A R ***********************
SemaphoreHandle_t wifi_mutex;

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
uint32_t stateTimer = 0;                                        //Main val
bool OLD = false, HGSP = false, CHP = false;                    //_

uint32_t releyOffTimer = 0, releyCommandProtTimer = 0,          //Реле
         UTIME = 0, DTIME = 0;                                  //
bool releyStatus = false, releyCommandStatus = false,           //
     releyDirection = true, motorSpeed = false,                 //
     lift_position = false, MOP = false, SINV = false;          //
int lastAction = 0;                                             //_

float Tin = 0, Tout = 0;                                        //Температура
bool VTIN = false, VTOUT = false, TSWAP = false;

bool waterVol = 0;                                              //Датчик воды

bool pumpStatus = false, PUMPM = false, USEPM = false,          //Помпа
     PUMPSENSINV = false, USEWS = false;
uint32_t DELAYPM = 0;                                           //_

uint32_t ledTimer = 0, brTimer = 0;                             //LED
bool ledAnimateStatus = false, brightnessStatus = false, INV = false;  //
byte C1 = 0, C2 = 0, C3 = 0, mainBR = 0, LEDN = 0, PARAM = 0,   //
     mLED = 0, SPEED = 0;                                       //
int curBR = 0;                                                  //_
bool LFLASH = false;

bool playStatus = false, SUSE = false, dfInit = false,          //Mp3
     SRND = false;
byte SVOL = 0, EQ = 0;                                          //
int fileCounts = 0;                                             //_

bool BUSE = false, blynkInitSt = false, blynkConnSt = false,    //Blynk
     blynkActionStatus = false;                                 //
int blynkAction = 0;                                            //_
bool blynk_init = true;

bool TGUSE = false, TGNOTIF = false;                            //Telegram

uint16_t pwm_vol = 0;                                           //OLD PWM

uint32_t optoTimer = 0;                                         //Оптопара
bool optoStatus = false;

uint32_t lastReceivedCode = 0, CODE1 = 0, CODE2 = 0,
         CODE3 = 0, CODE4 = 0;                                  //RF
bool USERF = false;

bool CHUDT = false;                                             //Проверка обновлений

bool DP22 = false, DP22U = false, DP22NO = false, DP22D = false, DP22C = false, DP22S = false, //DEVELOPMENT
     DP25 = false, DP25U = false, DP25NO = false, DP25D = false, DP25C = false, DP25S = false,
     DP26 = false, DP26U = false, DP26NO = false, DP26D = false, DP26C = false, DP26S = false;

bool MQUSE = false;


// Время и Дата
struct tm TD;
int TZ;

//Ночное охлаждение
bool NCOOL, effMute, nCoolStatus, WTHR;
int TGIST;
uint16_t TOPN, TCSE, TMOVE;
float WSPD;
struct {
  float W;
  bool R;
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


//36, 39, 4, 16?, 17?, 5, 3?, 1?
