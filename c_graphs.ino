
static unsigned long lastRotateTime = 0;
static int lastSentVolume = -1;  // ใช้เพื่อป้องกันส่งซ้ำ
const int maxVisibleTracks = 5;  // แสดง 5 รายการต่อหน้า
const int trackBoxW = 60;
const int trackBoxH = 40;
const int spacing = 10;
const char* baseMenuItems[] = { "Menu", "Mecanum", "MP3", "Calibrate Joy", "Wifi", "RO" };
char mainMenuItems[6][30];  // กำหนดเมนูแบบแก้ไขได้
int totalMainMenus = 6;
String playlistLabel = "Playlist";
String randomLabel = "Random";
const char* stopLabel = "Stop";
const char* menuLabel = "Menu";
const char* menuItems[] = {
  playlistLabel.c_str(),
  randomLabel.c_str(),
  stopLabel,
  menuLabel
};

// เมนู
void drawOverlayMenu() {
  static int lastAngle = -1;
  static int lastindexmenu = 0;
  static uint32_t lastMove = 0;
  static uint16_t colorEnd;
  const int maxVisibleItems = 3;
  int visibleStart = indexmenu - maxVisibleItems / 2;
  if (visibleStart < 0) visibleStart = 0;
  if (visibleStart > totalMainMenus - maxVisibleItems)
    visibleStart = totalMainMenus - maxVisibleItems;
  //sprite.fillSprite(displayColor3);  // พื้นหลังดำ
  // ขนาดและตำแหน่งของเมนู
  int menuX = 30;
  int menuY = 20;
  int menuW = 260;
  int menuH = 130;
  int cornerRadius = 10;
  int itemHeight = 25;
  int itemSpacing = 2;
  int headerHeight = 30;  // กำหนดความสูงของส่วนหัว
  const int iconSize = 48;
  const int iconsPerRow = 4;
  const int rowCount = (totalMainMenus + iconsPerRow - 1) / iconsPerRow;
  const int spacingX = (320 - (iconsPerRow * iconSize)) / (iconsPerRow + 1);
  const int spacingY = (170 - (rowCount * (iconSize + 20))) / (rowCount + 1);  // 20 = label

  if (resetvalue) {
    resetvalue = false;
    colorEnd = nextChassisColor();
    cleantft = true;
  }


  if (encoderValue != lastValue) {
    if (lastValue < encoderValue)
      ledtft = ledtft + 10;
    else
      ledtft = ledtft - 10;
    ledtft = constrain(ledtft, 0, 255);
    lastValue = encoderValue;
    ledcWrite(TFT_BL, ledtft);
    displayspeed(String(ledtft));
    cleantft = true;
  }
  if (millis() - lastMove > 150) {
    int joyX = convertJoystickValues(analogRead(JOY_L_X_PIN), valuemax1_min, valuemax1_max, false);
    if (joyX > 1000) {
      indexmenu--;
      lastMove = millis();
      indexmenu = constrain(indexmenu, 0, totalMainMenus - 1);
      cleantft = true;
    } else if (joyX < -1000) {
      indexmenu++;
      lastMove = millis();
      indexmenu = constrain(indexmenu, 0, totalMainMenus - 1);
      cleantft = true;
    } else {
      int joyY = convertJoystickValues(analogRead(JOY_L_Y_PIN), valuemax2_min, valuemax2_max, false);
      if (joyY > 1000) {
        if (indexmenu == 4)
          indexmenu = 0;
        else if (indexmenu == 5)
          indexmenu = 1;
        lastMove = millis();
        cleantft = true;
      } else if (joyY < -1000) {
        if (indexmenu == 0)
          indexmenu = 4;
        else if (indexmenu == 1)
          indexmenu = 5;
        lastMove = millis();
        cleantft = true;
      }
    }
  }
  if (cleantft) {
    cleantft = false;
    sprite.fillSprite(displayColor3);
    drawGlowPanel(0, 0, 320, 170, cornerRadius, displayColor2e, colorEnd);
    for (int i = 0; i < totalMainMenus; i++) {
      int row = i / iconsPerRow;
      int col = i % iconsPerRow;
      int x = spacingX + col * (iconSize + spacingX);
      int y = spacingY + row * (iconSize + 20 + spacingY);
      if (i == indexmenu) {
        drawGlowPanel(x - 6, y - 6, iconSize + 12, iconSize + 28, 6, displayColor2f, colorEnd);
      }
      sprite.pushImage(x, y, iconSize, iconSize, epd_bitmap_allArray[i]);
      sprite.setTextColor(i == indexmenu ? TFT_YELLOW : TFT_WHITE);
      sprite.setTextDatum(TC_DATUM);
      sprite.drawString(baseMenuItems[i], x + iconSize / 2, y + iconSize + 5, 2);
    }
    sprite.pushSprite(0, 35);
  }

  /*

    sprite.fillSprite(displayColor3);
    colorEnd = nextChassisColor();
   
    // วาดกรอบเมนูหลัก
    drawGlowPanel(menuX, menuY, menuW, menuH, cornerRadius, displayColor2e, colorEnd);  // Light Blue border
    // วาดพื้นหลังสำหรับส่วนหัว "Main Menu"
    drawGlowPanel(menuX + 2, menuY + 2, menuW - 4, headerHeight, cornerRadius - 1, displayColor6, colorEnd);  // ใช้สีไฮไลท์เป็นพื้นหลัง

    // วาดหัวข้อเมนู
    sprite.setTextColor(displayColor2a);  // เปลี่ยนสีตัวอักษรให้ตัดกับพื้นหลัง
    sprite.setTextDatum(TC_DATUM);
    sprite.drawString("Main Menu", menuX + menuW / 2, menuY + 15, 2);  // ปรับตำแหน่ง Y เล็กน้อย
    // วาดพื้นหลังภายในเมนู (ส่วนของรายการ)
    drawGlowPanel(menuX + 2, menuY + headerHeight + 2, menuW - 4, menuH - headerHeight - 4, cornerRadius - 1, displayColor5, colorEnd);  // Black inner
    // วาดรายการเมนู
    int startY = menuY + headerHeight + 15;
    for (int i = 0; i < maxVisibleItems; i++) {
      int menuIndex = visibleStart + i;
      if (menuIndex >= totalMainMenus) break;  // ป้องกันหลุด array
      int y = startY + i * (itemHeight + itemSpacing);
      if (menuIndex == indexmenu) {
        drawGlowPanel(menuX + 20, y - itemHeight / 2 - 2, menuW - 40, itemHeight + 4, 6, displayColor2f, colorEnd);
        sprite.setTextColor(displayColor3);
        sprite.setTextDatum(ML_DATUM);
        sprite.drawString("--->", menuX + 25, y + 2, 2);  // ขยับเข้าด้านในจาก 15 → 25
        sprite.setTextDatum(MR_DATUM);
        sprite.drawString("<---", menuX + menuW - 25, y + 2, 2);  // ขยับจาก -15 → -25
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(mainMenuItems[menuIndex], menuX + menuW / 2, y + 2, 2);
      } else {
        sprite.setTextColor(TFT_WHITE);
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(mainMenuItems[menuIndex], menuX + menuW / 2, y + 2, 2);
      }
    }
    */






  // drawGlowPanel(0, 0, 320, 170, cornerRadius, displayColor2e, colorEnd);


  /*
    // วาดพื้นหลังภายในเมนู (ส่วนของรายการ)
    sprite.fillRect(menuX + 2, menuY + headerHeight + 2, menuW - 4, menuH - headerHeight - 4, displayColor5);
    sprite.fillRect(menuX + 2, menuY + headerHeight + 2, menuW - 4, menuH - headerHeight - 4, colorEnd);
    drawGlowPanel(menuX + 2, menuY + headerHeight + 2, menuW - 4, menuH - headerHeight - 4, cornerRadius - 1, displayColor5, colorEnd);  // Black inner
    // วาดรายการเมนู
    int startY = menuY + headerHeight + 15;
    for (int i = 0; i < maxVisibleItems; i++) {
      int menuIndex = visibleStart + i;
      if (menuIndex >= totalMainMenus) break;  // ป้องกันหลุด array
      int y = startY + i * (itemHeight + itemSpacing);
      if (menuIndex == indexmenu) {
        drawGlowPanel(menuX + 20, y - itemHeight / 2 - 2, menuW - 40, itemHeight + 4, 6, displayColor2f, colorEnd);
        sprite.setTextColor(displayColor6);
        sprite.setTextDatum(ML_DATUM);
        sprite.drawString("--->", menuX + 25, y + 2, 2);  // ขยับเข้าด้านในจาก 15 → 25
        sprite.setTextDatum(MR_DATUM);
        sprite.drawString("<---", menuX + menuW - 25, y + 2, 2);  // ขยับจาก -15 → -25
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(mainMenuItems[menuIndex], menuX + menuW / 2, y + 2, 2);
      } else {
        sprite.setTextColor(TFT_WHITE);
        sprite.setTextDatum(MC_DATUM);
        sprite.drawString(mainMenuItems[menuIndex], menuX + menuW / 2, y + 2, 2);
      }
    }
    */
}

//mecanum
void displayDualMotorStatus(int mtrLFpwm, int mtrRFpwm, int mtrLRpwm, int mtrRRpwm) {



  static uint16_t colorEnd;
  int rightBaseX = 160;
  int backColor;

  uint16_t fontbackColor = displayColor3;
  String headerText;
  if (resetvalue) {
    resetvalue = false;
    colorEnd = nextChassisColor();
    cleantft = true;
  }

  if (encoderValue != lastValue) {
    if (lastValue < encoderValue)
      MAXSPEED = MAXSPEED + 100;
    else
      MAXSPEED = MAXSPEED - 100;
    MAXSPEED = constrain(MAXSPEED, 0, 4095);
    lastValue = encoderValue;
    displayspeed(String(MAXSPEED));
    cleantft = true;
  }

  if (cleantft) {
    cleantft = false;
    sprite.fillSprite(displayColor3);
    // Left half: graphMotorSpeed() ย่อขนาดและขยับให้อยู่ซ้าย
    drawGlowPanel(0, 0, 150, 160, 10, displayColor2b, colorEnd);
    //sprite.fillRect(0, 0, 150, 160, displayColor2b);
    sprite.setTextDatum(TC_DATUM);
    String headerText;
    switch (connectstatus) {
      case false:
        backColor = displayColor2a;
        fontbackColor = displayColor6;
        headerText = "Disconnect";
        sprite.setTextColor(displayColor6);
        sprite.drawString("Disconnect", 80, 10, 2);
        break;
      case true:
        backColor = displayColor2a;
        fontbackColor = displayColor3;
        headerText = "Mecanum";
        sprite.setTextColor(TFT_WHITE);
        sprite.drawString("PWM Graph", 80, 10, 2);
        break;
      default: backColor = displayColor2f; headerText = "Mecanum";
    }
    auto drawBar = [&](int x, int pwm) {
      int height = map(abs(pwm), 0, MAXSPEED, 0, 60);
      if (pwm > 0)
        drawGlowPanel(x, 110 - height, 12, height,5, displayColor5, colorEnd);
      else if (pwm < 0)
        drawGlowPanel(x, 110 - height, 12, height,5, displayColor4, colorEnd);
      else
        drawGlowPanel(x, 110, 12, 2,5, displayColor3, colorEnd);
    };

    drawBar(20, mtrLFpwm);
    drawBar(45, mtrRFpwm);
    drawBar(70, mtrLRpwm);
    drawBar(95, mtrRRpwm);
    sprite.setTextDatum(TL_DATUM);
    sprite.setTextColor(displayColor1);
    sprite.drawString("LF", 22, 120);
    sprite.drawString("RF", 47, 120);
    sprite.drawString("LR", 72, 120);
    sprite.drawString("RR", 97, 120);
    // Right half: displayMotorSpeed() แบบย่อ
    drawGlowPanel(rightBaseX, 0, 150, 160, 10, backColor, colorEnd);
    //sprite.fillRect(rightBaseX, 0, 150, 160, backColor);
    sprite.setTextDatum(TC_DATUM);
    sprite.setTextColor(fontbackColor);
    sprite.drawString(headerText, rightBaseX + 80, 10, 2);
    // วาดพื้นหลังตัวรถ (กรอบล้อมล้อ)
    int chassisX = rightBaseX + 35;                                                    // ขอบซ้ายกรอบ (ให้ห่างจากล้อ)
    int chassisY = 40;                                                                 // เริ่มตรงแนวล้อบน
    int chassisW = 70;                                                                 // กว้างกว่าล้อห่างกันนิดหน่อย
    int chassisH = 74;                                                                 // สูงพอดีกับล้อบนถึงล้อล่าง
    drawGlowPanel(chassisX, chassisY, chassisW, chassisH, 6, chassisColor, colorEnd);  // กรอบตัวรถ
    // วาดล้อ
    auto drawWheel = [&](int x, int y, int pwm) {
      uint16_t color = displayColor3;
      if (pwm > 0)
        drawGlowPanel(rightBaseX + x, y, 18, 24, 3, displayColor5, colorEnd);
      else if (pwm < 0)
        drawGlowPanel(rightBaseX + x, y, 18, 24, 3, displayColor4, colorEnd);
      else
        drawGlowPanel(rightBaseX + x, y, 18, 24, 3, displayColor3, colorEnd);
    };
    drawWheel(20, 40, mtrLFpwm);   // RF
    drawWheel(100, 40, mtrRFpwm);  // LF
    drawWheel(20, 90, mtrLRpwm);   // RR
    drawWheel(100, 90, mtrRRpwm);  // LR
    // แสดงค่า PWM
    sprite.setTextColor(TFT_WHITE);
    sprite.drawString(String(mtrLFpwm), rightBaseX + 25, 68, 2);
    sprite.drawString(String(mtrRFpwm), rightBaseX + 105, 68, 2);
    sprite.drawString(String(mtrLRpwm), rightBaseX + 25, 118, 2);
    sprite.drawString(String(mtrRRpwm), rightBaseX + 105, 118, 2);
    sprite.pushSprite(0, 35);
  }
}

// MP3
void displayMp3MenuUI() {
  static uint16_t colorEnd;
  static int lastAngle = -1;
  static uint32_t lastMove = 0;
  int leftWidth = 90;
  int rightX = leftWidth + 5;
  int rightW = 320 - leftWidth - 5;
  int rightY = 0;

  if (!cleantft) {
    if (resetvalue) {
      resetvalue = false;
      colorEnd = nextChassisColor();
      //sendMp3Command("GETDATA", "1");
      cleantft = true;
    }
    if (encoderValue != lastValue) {
      if (lastValue < encoderValue)
        ledtft = ledtft + 10;
      else
        ledtft = ledtft - 10;
      ledtft = constrain(ledtft, 0, 255);
      lastValue = encoderValue;
      ledcWrite(TFT_BL, ledtft);
      displayspeed(String(ledtft));
      cleantft = true;
    }

    if (millis() - lastMove > 150) {
      int joyY = convertJoystickValues(analogRead(JOY_L_Y_PIN), valuemax2_min, valuemax2_max, false);
      if (joyY > 1000) {
        mp3MenuIndex--;
        mp3MenuIndex = constrain(mp3MenuIndex, 0, menuCount - 1);
        lastMove = millis();
        cleantft = true;
      } else if (joyY < -1000) {
        mp3MenuIndex++;
        mp3MenuIndex = constrain(mp3MenuIndex, 0, menuCount - 1);
        lastMove = millis();
        cleantft = true;
      } else {
        int joyX = convertJoystickValues(analogRead(JOY_L_X_PIN), valuemax2_min, valuemax2_max, false);
        if (mp3MenuIndex == 0) {
          if (joyX > 1000) {
            selectedTrack--;
            selectedTrack = constrain(selectedTrack, 0, num1Files - 1);
            lastMove = millis();
            cleantft = true;
          } else if (joyX < -1000) {
            selectedTrack++;
            selectedTrack = constrain(selectedTrack, 0, num1Files - 1);
            lastMove = millis();
            cleantft = true;
          }
        } else {
          if (joyX > 1000) {
            VOLUME--;
            VOLUME = constrain(VOLUME, 1, 30);
            cleantft = true;
            lastMove = millis();
            if (wsControl.isConnected())
              wsControl.sendTXT("V," + String(VOLUME));
            updateVolumeBar();
          } else if (joyX < -1000) {
            VOLUME++;
            VOLUME = constrain(VOLUME, 1, 30);
            cleantft = true;
            lastMove = millis();
            if (wsControl.isConnected())
              wsControl.sendTXT("V," + String(VOLUME));
            updateVolumeBar();
          }
        }
      }
    }
  }
  if (cleantft) {
    cleantft = false;
    // ล้าง sprite ด้วยพื้นหลังสีดำ
    sprite.fillSprite(displayColor3);
    // *** ส่วนเมนูด้านซ้าย ***
    drawGlowPanel(0, 0, leftWidth, 170, 5, displayColor2d, colorEnd);  // พื้นหลังเมนูซ้าย
    // หัวข้อเมนูซ้าย
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextDatum(TC_DATUM);
    //sprite.drawString("Menu", leftWidth / 2, 10, 2);
    // รายการเมนู
    int menuStartY = 35;
    int itemHeight = 25;
    int itemSpacing = 5;
    for (int i = 0; i < menuCount; i++) {
      int y = menuStartY + i * (itemHeight + itemSpacing);
      if (i == mp3MenuIndex) {
        // ไฮไลต์เมนูที่เลือก
        drawGlowPanel(5, y - itemHeight / 2 - 2, leftWidth - 10, itemHeight + 4, 5, displayColor2f, colorEnd);
        sprite.setTextColor(TFT_WHITE);
        sprite.setTextDatum(MC_DATUM);
        if (i == 0)
          sprite.drawString("Playlist(" + String(num1Files) + ")", leftWidth / 2, y + 2, 2);
        else if (i == 1)
          sprite.drawString("Random(" + String(num2Files) + ")", leftWidth / 2, y + 2, 2);
        else
          sprite.drawString(menuItems[i], leftWidth / 2, y + 2, 2);
      } else {
        sprite.setTextColor(TFT_WHITE);
        sprite.setTextDatum(MC_DATUM);
        if (i == 0)
          sprite.drawString("Playlist(" + String(num1Files) + ")", leftWidth / 2, y + 2, 2);
        else if (i == 1)
          sprite.drawString("Random(" + String(num2Files) + ")", leftWidth / 2, y + 2, 2);
        else
          sprite.drawString(menuItems[i], leftWidth / 2, y + 2, 2);
      }
    }
    // *** ส่วนแสดงเนื้อหาด้านขวา ***
    // พื้นหลังของพื้นที่ด้านขวา
    if (mp3MenuIndex == 0) {
      // เมนู Playlist: วาด tracklist แนวนอนที่ดูดีขึ้น
      const int maxVisibleTracks = 5;
      const int trackBoxW = 50;   // เพิ่มความกว้าง
      const int trackBoxH = 25;   // ลดความสูง
      const int spacing = 15;     // เพิ่มระยะห่าง
      const int trackListY = 50;  // ตำแหน่งแนวตั้งของรายการเพลง
      int visibleStart = selectedTrack - maxVisibleTracks / 2;
      if (visibleStart < 0) visibleStart = 0;
      if (visibleStart > num1Files - maxVisibleTracks) visibleStart = num1Files - maxVisibleTracks;
      // วาดพื้นหลังสำหรับ Tracklist
      drawGlowPanel(rightX + 10, rightY + 10, rightW - 20, 140, 8, displayColor6, colorEnd);
      for (int i = 0; i < maxVisibleTracks; i++) {
        int trackIndex = visibleStart + i;
        if (trackIndex >= 0 && trackIndex < num1Files) {
          int x = rightX + 20 + i * (trackBoxW + spacing);
          int y = trackListY + (trackBoxH / 2) + 2;
          String label = "";
          if (trackIndex + 1 < 10) label = "0" + String(trackIndex + 1);
          else label = String(trackIndex + 1);
          sprite.setTextDatum(MC_DATUM);
          if (trackIndex == selectedTrack) {
            drawGlowPanel(x - 5, y - (trackBoxH / 2) - 3, trackBoxW + 10, trackBoxH + 6, 5, displayColor2f, colorEnd);
            sprite.setTextColor(TFT_WHITE);
            sprite.drawString(label, x + trackBoxW / 2, y, 2);
          } else {
            sprite.setTextColor(TFT_WHITE, displayColor3);
            sprite.drawString(label, x + trackBoxW / 2, y, 1);
          }
        }
      }
    } else {
      if (vzstatus)
        drawGlowPanel(rightX, rightY + 5, rightW, 160, 8, displayColor1, colorEnd);
      // เมนู Random, Stop, หรือ Exit: แสดง visualizer ที่จัดวางดีขึ้น
      (rightX + 10, rightY + 10, rightW - 20, 140, 8, displayColor2c);
    }
    // วาด control hints ด้านล่าง
    sprite.setTextDatum(TL_DATUM);
    sprite.setTextColor(TFT_WHITE);
    // แสดง Volume พร้อม Icon (ปรับปรุงตำแหน่ง)
    sprite.setTextColor(TFT_YELLOW, displayColor3);
    sprite.drawString("Vol:", rightX + 10, rightY + 145, 1);
    int volBarWidth = map(VOLUME, 0, 30, 0, rightW - 50);
    sprite.fillRect(rightX + 40, rightY + 145, volBarWidth, 8, TFT_GREEN);
    sprite.drawRect(rightX + 40, rightY + 145, rightW - 50, 8, TFT_WHITE);
    // ส่ง sprite ขึ้นหน้าจอ
    sprite.pushSprite(0, 35);
  }

  if (mp3MenuIndex == 0)
    drawVisualizesmall(rightW, rightY, rightX);
  else
    drawVisualizebig(rightW, rightY, rightX);

  // แสดงผล
}
void displayJoystickMonitor() {
  static uint16_t colorEnd;
  if (!ledOn) {
    digitalWrite(LED_PIN, HIGH);
    ledOn = true;
    ledOffTimer = millis();
  }
  int x1 = analogRead(JOY_L_X_PIN);
  int y1 = analogRead(JOY_L_Y_PIN);
  int x2 = analogRead(JOY_R_X_PIN);

  minX1 = min(minX1, x1);
  maxX1 = max(maxX1, x1);
  minY1 = min(minY1, y1);
  maxY1 = max(maxY1, y1);
  minX2 = min(minX2, x2);
  maxX2 = max(maxX2, x2);
  if (resetvalue) {
    resetvalue = false;
    colorEnd = nextChassisColor();
  }
  sprite.fillSprite(displayColor3);
  drawGlowPanel(10, 10, 300, 150, 10, displayColor2b, colorEnd);
  sprite.setTextDatum(TC_DATUM);
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString("Joystick Monitor", 160, 20, 2);
  sprite.setTextDatum(TL_DATUM);
  sprite.setTextColor(displayColor1);
  // คอลัมน์ซ้าย: ค่าปัจจุบัน
  sprite.drawString("X1:", 20, 50, 2);
  sprite.drawString("Y1:", 20, 70, 2);
  sprite.drawString("X2:", 20, 90, 2);
  //sprite.drawString("Y2:", 20, 110, 2);
  // คอลัมน์กลาง: ค่าปัจจุบัน
  sprite.setTextColor(TFT_WHITE);
  sprite.drawString(String(x1), 60, 50, 2);
  sprite.drawString(String(y1), 60, 70, 2);
  sprite.drawString(String(x2), 60, 90, 2);
  //sprite.drawString(String(y2), 60, 110, 2);
  // คอลัมน์ขวา: min/max
  sprite.setTextColor(TFT_YELLOW);
  sprite.drawString("Min", 130, 30, 2);
  sprite.drawString("Max", 190, 30, 2);
  sprite.drawString(String(minX1), 130, 50, 2);
  sprite.drawString(String(maxX1), 190, 50, 2);
  sprite.drawString(String(minY1), 130, 70, 2);
  sprite.drawString(String(maxY1), 190, 70, 2);
  sprite.drawString(String(minX2), 130, 90, 2);
  sprite.drawString(String(maxX2), 190, 90, 2);
  // เพิ่ม MAXSPEED ด้านล่าง
  sprite.setTextColor(TFT_CYAN);
  sprite.drawString("MAXSPEED: " + String(MAXSPEED), 130, 120, 2);
  sprite.pushSprite(0, 35);
}


void drawLoginUI() {

  // พื้นหลัง sprite
  sprite.fillSprite(TFT_NAVY);

  // กล่อง Login ตรงกลาง
  int boxX = 30, boxY = 20, boxW = 260, boxH = 130;
  sprite.fillRoundRect(boxX, boxY, boxW, boxH, 8, TFT_WHITE);
  sprite.drawRoundRect(boxX, boxY, boxW, boxH, 8, TFT_BLUE);

  // Label “ID”
  sprite.setTextColor(TFT_BLACK);
  sprite.setTextDatum(TL_DATUM);
  sprite.drawString("ID", boxX + 20, boxY + 15, 2);

  // ช่องใส่ ID
  sprite.drawRect(boxX + 60, boxY + 12, 160, 18, TFT_DARKGREY);

  // Checkbox “keep”
  sprite.drawRect(boxX + 230, boxY + 15, 10, 10, TFT_DARKGREY);
  sprite.drawString("keep", boxX + 245, boxY + 12, 1);

  // Label “Password”
  sprite.drawString("Password", boxX + 20, boxY + 45, 2);

  // ช่องใส่ Password
  sprite.drawRect(boxX + 100, boxY + 42, 120, 18, TFT_DARKGREY);

  // ปุ่ม login
  sprite.fillRoundRect(boxX + 100, boxY + 90, 50, 20, 3, TFT_BLUE);
  sprite.setTextColor(TFT_WHITE);
  sprite.setTextDatum(MC_DATUM);
  sprite.drawString("login", boxX + 125, boxY + 100, 2);

  // ปุ่ม exit
  sprite.fillRoundRect(boxX + 160, boxY + 90, 50, 20, 3, TFT_LIGHTGREY);
  sprite.setTextColor(TFT_BLACK);
  sprite.drawString("exit", boxX + 185, boxY + 100, 2);

  sprite.pushSprite(0, 35);
}
