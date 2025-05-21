#include <ArduinoOTA.h>
#include <WiFiMulti.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <nRF24L01.h>
#include <WebSocketsClient_Generic.h>
#include <ArduinoJson.h>
#include <RF24.h>
#include <esp_wifi.h>
#define CLK 22
#define DT 26
#define rotaryButtonPin 21
#define JOY_L_X_PIN 36
#define JOY_L_Y_PIN 39
#define JOY_R_X_PIN 34
//#define JOY_R_Y_PIN 35
#define BUZZER_PIN 25
#define LED_PIN 12
#define normalButtonPin 27
// Display colors
#define displayColor1 0xFFFF      // White
#define displayColor2a 0x1E1E     // Cyan
#define displayColor2b 0x5566     // Green
#define displayColor2c 0xE3E3     // Orange
#define displayColor2d 0xFAFA     // Violet
#define displayColor2e 0xBEBE     // Light Blue
#define displayColor2f 0xE6E6     // Yellow
#define displayColor3 0x0000      // Black
#define displayColor4 0x2727      // Green Bar
#define displayColor5 0x1B1B      // Blue Bar
#define displayColor6 0xE8E8      // Red
const byte address[6] = "00001";  // ที่อยู่ของตัวส่งและตัวรับ
int ledtft = 255;
RF24 radio(17, 5);  // CSN=5, CE=17
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

WebSocketsClient wsControl;
#define WS_SERVER "192.168.1.211"
#define WS_PORT 81
WiFiMulti wifiMulti;

/*
#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   4
#define TFT_DC    2
#define TFT_RST   16
#define TFT_BL   15
*/
//#define TFT_BL   15
// Define a data structure for received data

bool vzstatus = false;
uint8_t VOLUME = 0;
uint16_t num1Files = 0;
uint16_t num2Files = 0;

typedef struct __attribute__((packed)) struct_message_rcv {
  int16_t mtrRF_PWM;
  int16_t mtrLF_PWM;
  int16_t mtrRR_PWM;
  int16_t mtrLR_PWM;
} struct_message_rcv;
struct_message_rcv rcvData;


int16_t mtrRFpwmValue = 0;
int16_t mtrLFpwmValue = 0;
int16_t mtrRRpwmValue = 0;
int16_t mtrLRpwmValue = 0;

typedef struct __attribute__((packed)) struct_message_xmit {
  int16_t rf_speed;
  int16_t lf_speed;
  int16_t rr_speed;
  int16_t lr_speed;
  bool pbSwitch;
  uint16_t speedmax;
  uint8_t carMode;
} struct_message_xmit;
struct_message_xmit xmitData;

volatile bool pbSwitch = false;
int16_t rf_speed = 0;
int16_t lf_speed = 0;
int16_t rr_speed = 0;
int16_t lr_speed = 0;
uint16_t speedmax = 2600;
uint8_t carMode = 0;

uint16_t MAXSPEED = 2600;
int minX1 = 9999, maxX1 = 0;
int minY1 = 9999, maxY1 = 0;
int minX2 = 9999, maxX2 = 0;
uint16_t valuemax1_max = 0;
uint16_t valuemax1_min = 0;
uint16_t valuemay1_max = 0;
uint16_t valuemay1_min = 0;
uint16_t valuemax2_max = 0;
uint16_t valuemax2_min = 0;
uint16_t chassisColors[] = {
  displayColor1, displayColor2a, displayColor2b, displayColor2c,
  displayColor2d, displayColor2e, displayColor2f, displayColor3, displayColor4, displayColor5, displayColor6
};
// 2. ตัวแปรเก็บสถานะสีปัจจุบัน
int currentColorIndex = 0;
uint16_t chassisColor = chassisColors[currentColorIndex];
// 3. ฟังก์ชันเปลี่ยนสี (เรียกตอนกดปุ่ม)
uint16_t nextChassisColor() {
  currentColorIndex = random(0, 11);
  chassisColor = chassisColors[currentColorIndex];
  return chassisColor;
}
short MainmenuIndex = 0;  //ตัวชี้ตำแหน่ง ui ปัจจุบัน
bool backlightOn = true;
volatile bool connectstatus = false;
bool resetvalue = true;
String connectInfo = "NO INFO";
bool ledOn = false;
unsigned long ledOffTimer = 0;
volatile int encoderValue = 0;
int lastValue = 0;
volatile int lastCLKState;
bool cleantft = false;
uint8_t mp3MenuIndex = 0;
uint8_t selectedTrack = 0;
uint8_t menuCount = 4;
int indexmenu = 0;
//WiFi.disconnect(true);
//WiFi.setSleep(false);

void setupwifi() {
  bannerinfo("Connect wifi..");
  WiFi.enableIPv6();
  WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP
  wifiMulti.addAP("ESP32_AP", "999999999");
  wifiMulti.addAP("Milin_2.4G", "999999999");
  Serial.println("Connecting Wifi...");

  unsigned long startAttemptTime = millis();
  while (wifiMulti.run() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
    delay(100);
  }
  WiFi.linkLocalIPv6();
  /*
  while (WiFi.linkLocalIPv6() == "") {
    Serial.println("Waiting for IPv6 Address...");
    delay(100);
  }
  */
  ArduinoOTA.onStart([]() {
              String type;
              if (ArduinoOTA.getCommand() == U_FLASH) {
                type = "sketch";
              } else {  // U_SPIFFS
                type = "filesystem";
              }
              // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
              Serial.println("Start updating " + type);
            })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();
  // แสดง IPv6 Address
  Serial.print("IPv6 Address: ");
  Serial.println(WiFi.linkLocalIPv6());

  wsControl.begin(WS_SERVER, WS_PORT, "/");
  wsControl.onEvent(onControlEvent);
  // wsControl.setReconnectInterval(5000);
  //wsControl.enableHeartbeat(15000, 3000, 2);
}

const unsigned long longPressThreshold = 2500;
volatile bool rotaryButtonPressedInterrupt = false;
volatile bool normalButtonPressedInterrupt = false;
volatile unsigned long normalButtonPressStartTime = 0;
volatile bool normalButtonLongPressActive = false;

volatile bool mp3Menuselect = false;
void IRAM_ATTR readButtonSWITCH() {
  static bool lastNormalButtonState = HIGH;
  bool currentNormalButtonState = digitalRead(normalButtonPin);

  if (currentNormalButtonState == LOW && lastNormalButtonState == HIGH) {
    normalButtonPressedInterrupt = true;
    normalButtonPressStartTime = millis();
    normalButtonLongPressActive = false;  // รีเซ็ตสถานะกดค้างเมื่อมีการกดใหม่
  }
  lastNormalButtonState = currentNormalButtonState;
}

void IRAM_ATTR readEncoder() {
  int currentCLKState = digitalRead(CLK);
  if (currentCLKState != lastCLKState && currentCLKState == LOW) {
    if (digitalRead(DT) != currentCLKState) {
      encoderValue = encoderValue + 1;
    } else {
      encoderValue = encoderValue - 1;
    }
  }
  lastCLKState = currentCLKState;
}

void playClickSound() {
  tone(BUZZER_PIN, 880, 80);  // เล่นเสียง 2kHz นาน 100ms
  noTone(BUZZER_PIN);         // หยุดเสียง (เผื่อบางรุ่นเสียงค้าง)
}
void playvibration() {
  tone(BUZZER_PIN, 2000, 100);  // เล่นเสียง 2kHz นาน 100ms
  delay(100);                   // รอให้เสียงจบ
  noTone(BUZZER_PIN);           // หยุดเสียง (เผื่อบางรุ่นเสียงค้าง)
}
void drawHeader(uint16_t colorEnd) {
  drawGlowhead(0, 0, 320, 30, 5, displayColor6, colorEnd);  // สีพื้นหัวจอ
  tft.setTextColor(TFT_YELLOW);
  tft.setTextDatum(MC_DATUM);                                 // กลางจอ
  tft.drawString("K ROBOT", 160, 25, 4);                      // วาดตรงกลางด้านบน
  drawGlowhead(0, 205, 320, 30, 5, displayColor6, colorEnd);  // สีพื้นหัวจอ
}

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DT, INPUT_PULLUP);

  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(normalButtonPin, INPUT);         // ปุ่มกดขวา
  pinMode(rotaryButtonPin, INPUT_PULLUP);  //ปุ่มกดของ Encoder
                                           //pinMode(TFT_BL, OUTPUT);
  pinMode(TFT_BL, OUTPUT);
  ledcAttach(TFT_BL, 5000, 8);
  ledcWrite(TFT_BL, 255);

  if (!radio.begin()) {
    Serial.println("ไม่พบโมดูล nRF24L01");
    while (1)
      ;
  }
  radio.setChannel(108);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_LOW);
  radio.enableAckPayload();
  radio.enableDynamicPayloads();
  radio.setAutoAck(true);
  //radio.setPayloadSize(sizeof(xmitData));
  radio.stopListening(address);
  radio.openWritingPipe(address);

  //digitalWrite(TFT_BL, HIGH);  // เปิดจอ
  backlightOn = true;
  tft.init();
  tft.setRotation(3);
  drawHeader(nextChassisColor());
  sprite.createSprite(320, 170);

  connectInfo = "Waiting for car";
  analogReadResolution(12);
  lastCLKState = digitalRead(CLK);
  //attachInterrupt(digitalPinToInterrupt(normalButtonPin), readButtonSWITCH, FALLING);  //	ขอบขาลง (HIGH → LOW)	ตอนกดปุ่มลง (กับ INPUT_PULLUP)
  //attachInterrupt(digitalPinToInterrupt(rotaryButtonPin), handleSWChange, CHANGE);     //ขอบขาขึ้น (LOW → HIGH)	ตอนปล่อยปุ่ม (หากใช้ INPUT_PULLUP)
  attachInterrupt(digitalPinToInterrupt(CLK), readEncoder, CHANGE);
  unsigned long startAttemptTime = millis();
  bannerinfo("Calibate..Joy");
  delay(1000);
  while (millis() - startAttemptTime < 5000) {
    displayJoystickMonitor();
    delay(100);
  }
  valuemax1_max = calcentervalue(minX1, maxX1, true);
  valuemax1_min = calcentervalue(minX1, maxX1, false);
  valuemay1_max = calcentervalue(minY1, maxY1, true);
  valuemay1_min = calcentervalue(minY1, maxY1, false);
  valuemax2_max = calcentervalue(minX2, maxX2, true);
  valuemax2_min = calcentervalue(minX2, maxX2, false);
  bannerinfo("Sucess..");
  delay(1000);
  MainmenuIndex = 1;
  resetvalue = true;
  pbSwitch = false;
}


void carstop() {
  rf_speed = 0;
  rr_speed = 0;
  lr_speed = 0;
  lf_speed = 0;
}
unsigned long lastRecvTime = 0;
void loop() {
   unsigned long now = millis();
  ArduinoOTA.handle();
  wsControl.loop();
  int joyxaxis = 0;
  int joyyaxis = 0;
  int joyXaxis2 = 0;

  if (now - lastRecvTime > 500)
    connectstatus = false;
  else
    connectstatus = true;


  carstop();
  eventbutton();
  switch (MainmenuIndex) {
    case 0:
      drawOverlayMenu();
      break;
    case 1:
      //UI Display

      if (radio.isAckPayloadAvailable()) {
        byte len = radio.getDynamicPayloadSize();
        radio.read(&rcvData, len);
        mtrRFpwmValue = rcvData.mtrRF_PWM;
        mtrLFpwmValue = rcvData.mtrLF_PWM;
        mtrRRpwmValue = rcvData.mtrRR_PWM;
        mtrLRpwmValue = rcvData.mtrLR_PWM;
      }
      displayDualMotorStatus(mtrRFpwmValue, mtrLFpwmValue, mtrRRpwmValue, mtrLRpwmValue);
      if (mtrRFpwmValue == 0 && mtrLFpwmValue == 0 && mtrRRpwmValue == 0 && mtrLRpwmValue == 0)
        cleantft = false;
      else
        cleantft = true;
      if (!pbSwitch) {
        joyxaxis = convertJoystickValues(analogRead(JOY_L_X_PIN), valuemax1_min, valuemax1_max, false);
        joyyaxis = convertJoystickValues(analogRead(JOY_L_Y_PIN), valuemay1_min, valuemay1_max, false);
        joyXaxis2 = convertJoystickValues(analogRead(JOY_R_X_PIN), valuemax2_min, valuemax2_max, false);  // หมุน



        rf_speed = constrain(joyyaxis + joyxaxis - joyXaxis2, -MAXSPEED, MAXSPEED);  // RF
        lf_speed = constrain(joyyaxis - joyxaxis + joyXaxis2, -MAXSPEED, MAXSPEED);  // LF
        rr_speed = constrain(joyyaxis - joyxaxis - joyXaxis2, -MAXSPEED, MAXSPEED);  // RR
        lr_speed = constrain(joyyaxis + joyxaxis + joyXaxis2, -MAXSPEED, MAXSPEED);  // LR
      }
      xmitData.rf_speed = rf_speed;
      xmitData.lf_speed = lf_speed;
      xmitData.rr_speed = rr_speed;
      xmitData.lr_speed = lr_speed;
      xmitData.pbSwitch = pbSwitch;
      xmitData.speedmax = MAXSPEED;
      xmitData.carMode = MainmenuIndex;
      if (radio.write(&xmitData, sizeof(xmitData)))
        lastRecvTime = millis();
      break;
    case 2:
      displayMp3MenuUI();
      break;
    case 3:
      displayJoystickMonitor();
      break;
    case 4:
      break;
    case 5:
      drawLoginUI();
      break;
  }
  timeroff();
  // delay(20);
}
