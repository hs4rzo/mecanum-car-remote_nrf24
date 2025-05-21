
uint8_t getRed(uint16_t color) {
  return (color >> 11) & 0x1F;
}

uint8_t getGreen(uint16_t color) {
  return (color >> 5) & 0x3F;
}

uint8_t getBlue(uint16_t color) {
  return color & 0x1F;
}


void drawGlowhead(
  int x, int y,  // ตำแหน่งมุมบนซ้ายของกล่อง
  int w, int h,  // ขนาดกล่อง
  int radius,    // ยังไม่ใช้ radius ที่นี่ แต่เก็บไว้เผื่อใช้งาน
  uint16_t colorStart,
  uint16_t colorEnd) {
  int glowThickness = 3;
  uint16_t borderColor = displayColor1;  // สีขอบเรืองแสง

  // ไล่ระดับสีพื้นหลังในกล่อง
  for (int i = 0; i < h; i++) {
    uint8_t r = getRed(colorStart) + (getRed(colorEnd) - getRed(colorStart)) * i / h;
    uint8_t g = getGreen(colorStart) + (getGreen(colorEnd) - getGreen(colorStart)) * i / h;
    uint8_t b = getBlue(colorStart) + (getBlue(colorEnd) - getBlue(colorStart)) * i / h;

    uint16_t grad = tft.color565(r << 3, g << 2, b << 3);  // เปลี่ยนจาก sprite เป็น tft
    tft.drawFastHLine(x, y + i, w, grad);                  // วาดตรงลงจอ
  }

  // หากต้องการเพิ่มขอบเรืองแสงหรือเอฟเฟกต์ radius เพิ่มเติม สามารถเติมทีหลังได้
}

void drawGlowPanel(
  int x, int y,  // ตำแหน่งมุมบนซ้ายของกล่อง
  int w, int h,  // ขนาดกล่อง
  int radius,
  uint16_t colorStart,
  uint16_t colorEnd) {

  // สีเริ่มต้น (ด้านบน)
  //uint16_t colorEnd= nextChassisColor();      // สีสิ้นสุด (ด้านล่าง)
  uint16_t borderColor = displayColor1;  // สีขอบเรืองแสง
  int glowThickness = 3;
  // ไล่ระดับสีพื้นหลังในกล่อง
  for (int i = 0; i < h; i++) {
    uint8_t r = getRed(colorStart) + (getRed(colorEnd) - getRed(colorStart)) * i / h;
    uint8_t g = getGreen(colorStart) + (getGreen(colorEnd) - getGreen(colorStart)) * i / h;
    uint8_t b = getBlue(colorStart) + (getBlue(colorEnd) - getBlue(colorStart)) * i / h;
    uint16_t grad = sprite.color565(r << 3, g << 2, b << 3);
    sprite.drawFastHLine(x, y + i, w, grad);
  }

  // ขอบเรืองแสง
  for (int i = 0; i < glowThickness; i++) {
    sprite.drawRoundRect(x - i, y - i, w + i * 2, h + i * 2, radius, borderColor);
  }
}
void clearRectPanelArea(int x, int y, int w, int h, uint16_t bgColor = displayColor3) {
  sprite.fillRect(x, y, w, h, bgColor);
}

void fillRectPanel(int x, int y, int w, int h, uint16_t color, bool withBorder = true) {
  clearRectPanelArea(x, y, w, h);
  sprite.fillRect(x, y, w, h, color);
  if (withBorder) {
    sprite.drawRect(x - 1, y - 1, w + 2, h + 2, displayColor1);  // ขอบบางๆ
  }
}

void splashScreen(String messageString, bool errStat) {
  int screenWidth = 320;
  int screenHeight = 170;

  uint16_t colorStart = errStat ? displayColor6 : displayColor2b;
  uint16_t colorEnd = displayColor3;
  uint16_t titleColor = displayColor1;
  uint16_t msgColor = TFT_WHITE;

  // พื้นหลังไล่สี
  for (int y = 0; y < screenHeight; y++) {
    uint8_t r = getRed(colorStart) + (getRed(colorEnd) - getRed(colorStart)) * y / screenHeight;
    uint8_t g = getGreen(colorStart) + (getGreen(colorEnd) - getGreen(colorStart)) * y / screenHeight;
    uint8_t b = getBlue(colorStart) + (getBlue(colorEnd) - getBlue(colorStart)) * y / screenHeight;
    uint16_t gradientColor = sprite.color565(r << 3, g << 2, b << 3);
    sprite.drawFastHLine(0, y, screenWidth, gradientColor);
  }

  // ขอบเรืองแสง
  int borderWidth = 4;
  for (int i = 0; i < borderWidth; i++) {
    sprite.drawRoundRect(i, i, screenWidth - 1 - (i * 2), screenHeight - 1 - (i * 2), 8, displayColor1);
  }

  // ไอคอน
  int iconSize = 40;
  int iconX = screenWidth / 2 - iconSize / 2;
  int iconY = 20;
  if (errStat) {
    sprite.fillCircle(iconX + iconSize / 2, iconY + iconSize / 2, iconSize / 2, TFT_RED);
    sprite.setTextColor(TFT_WHITE);
    sprite.setTextDatum(MC_DATUM);
    sprite.drawString("!", iconX + iconSize / 2, iconY + iconSize / 2, 4);
  } else {
    sprite.drawLine(iconX + 7, iconY + iconSize - 7, iconX + 7, iconY + 7, TFT_CYAN);
    sprite.drawLine(iconX + 20, iconY + iconSize - 10, iconX + 20, iconY + 10, TFT_CYAN);
    sprite.drawLine(iconX + 33, iconY + iconSize - 13, iconX + 33, iconY + 13, TFT_CYAN);
  }

  // หัวข้อ
  sprite.setTextColor(titleColor);
  sprite.setTextDatum(TC_DATUM);
  sprite.drawString(errStat ? "ERROR" : "NRF-24", screenWidth / 2, iconY + iconSize + 8, 4);
  sprite.drawString(errStat ? "CONNECTING" : "Mecanum Car", screenWidth / 2, iconY + iconSize + 40, 2);

  // กล่องข้อความ
  int boxWidth = screenWidth - 60;
  int boxHeight = 50;
  int boxX = 30;
  int boxY = screenHeight - boxHeight - 15;
  int glowThickness = 3;
  uint16_t glowColor = displayColor1;
  uint16_t boxBgColor = sprite.color565(40, 40, 40);

  for (int i = 0; i < glowThickness; i++) {
    sprite.drawRoundRect(boxX - i, boxY - i, boxWidth + (i * 2), boxHeight + (i * 2), 8, glowColor);
  }
  sprite.fillRoundRect(boxX, boxY, boxWidth, boxHeight, 8, boxBgColor);

  sprite.setTextColor(msgColor);
  sprite.setTextDatum(TC_DATUM);
  sprite.drawString(messageString, screenWidth / 2, boxY + boxHeight / 2, 2);

  sprite.pushSprite(0, 35);
}

void controlInfo() {
  // System information display
}

void updateMenuStatus() {
  for (int i = 0; i < totalMainMenus; i++) {
    if (i == 3) {  // ตำแหน่งของเมนู ON-OFF WIFI
                   //  bool wifiOn = (WiFi.status() == WL_CONNECTED);
                   // snprintf(mainMenuItems[i], sizeof(mainMenuItems[i]), "WIFI: %s", wifiOn ? "ON" : "OFF");
      strncpy(mainMenuItems[i], baseMenuItems[i], sizeof(mainMenuItems[i]));
    } else {
      strncpy(mainMenuItems[i], baseMenuItems[i], sizeof(mainMenuItems[i]));
    }
  }
}

void timeroff() {
  if (millis() - ledOffTimer > 5000 && ledOn) {
    digitalWrite(LED_PIN, LOW);
    ledOn = false;
  }
}
void ledlight() {
  if (!ledOn) {
    digitalWrite(LED_PIN, HIGH);
    ledOn = true;
    ledOffTimer = millis();
  }
}

void bannerinfo(String info) {
  int boxWidth = 280;
  int boxHeight = 70;
  int boxX = (320 - boxWidth) / 2;
  int boxY = (170 - boxHeight) / 2;

  // วาดพื้นหลังโค้งตรงกลางหน้าจอ
  tft.fillRoundRect(boxX, boxY, boxWidth, boxHeight, 10, 0xE8E8);  // สีพื้นหลัง

  // ตั้งค่าฟอนต์และสีข้อความ
  tft.setTextColor(0x0000, 0xE8E8);  // ดำบนพื้นเทา
  tft.setTextDatum(MC_DATUM);
  tft.drawString(info, 160, boxY + boxHeight / 2, 4);  // ใช้ฟอนต์ขนาดใหญ่พอประมาณ
}
void displayspeed(String number) {
  int boxWidth = 300;
  int boxHeight = 60;
  int boxX = (320 - boxWidth) / 2;
  int boxY = (170 - boxHeight) / 2;

  // วาดกรอบพื้นหลังโค้ง
  tft.fillRoundRect(boxX, boxY, boxWidth, boxHeight, 10, 0xE8E8);

  // ตั้งค่าฟอนต์ใหญ่สุดใน default (Font 7)
  tft.setTextColor(0xE6E6, 0xE8E8);  // สีตัวเลข / สีพื้นหลัง
  tft.setTextDatum(MC_DATUM);        // ให้อยู่กลางกล่อง
  tft.drawString(String(number), 160, boxY + boxHeight / 2, 7);
}

void displayVolumeLevel(int volume) {
  int boxWidth = 300;
  int boxHeight = 60;
  int boxX = (320 - boxWidth) / 2;
  int boxY = (170 - boxHeight) / 2;

  // พื้นหลังกล่อง
  tft.fillRoundRect(boxX, boxY, boxWidth, boxHeight, 10, displayColor6);  // สีแดงอ่อน

  // วาดตัวเลขระดับเสียง
  tft.setTextColor(displayColor2f, displayColor6);  // สีเหลือง / สีพื้น
  tft.setTextDatum(MC_DATUM);
  tft.drawString("Vol: " + String(volume), 160, boxY + 12, 4);

  // วาดแถบกราฟเสียง
  int barWidth = map(volume, 0, 30, 0, 200);
  int barX = 60;
  int barY = boxY + 38;
  tft.drawRect(barX, barY, 200, 10, TFT_WHITE);
  tft.fillRect(barX, barY, barWidth, 10, TFT_GREEN);

  // วาดไอคอนเสียง
  int iconX = 270;
  int iconY = boxY + 15;

  if (volume == 0) {
    // มิวท์ (Mute)
    tft.fillRect(iconX, iconY + 5, 5, 10, TFT_RED);
    tft.drawLine(iconX, iconY + 5, iconX + 10, iconY + 15, TFT_RED);
  } else if (volume <= 10) {
    // เสียงเบา
    tft.fillCircle(iconX, iconY + 10, 3, TFT_CYAN);
  } else if (volume <= 20) {
    // เสียงกลาง
    tft.fillCircle(iconX, iconY + 10, 4, TFT_YELLOW);
    tft.drawCircle(iconX, iconY + 10, 6, TFT_YELLOW);
  } else {
    // เสียงดัง
    tft.fillCircle(iconX, iconY + 10, 5, TFT_GREEN);
    tft.drawCircle(iconX, iconY + 10, 7, TFT_GREEN);
    tft.drawCircle(iconX, iconY + 10, 9, TFT_GREEN);
  }
}

void drawVisualizebig(int rightW, int rightY, int rightX) {

  if (vzstatus) {
    int barCount = 15;  // ปรับจำนวนแท่ง
    int barSpacing = 5;
    int availableWidth = rightW - 40;
    int barWidth = (availableWidth - (barCount - 1) * barSpacing) / barCount;
    int maxHeight = 100;

    int baseY = rightY + 145;  // ฐาน visualizer ใกล้ด้านล่าง
    int i = random(barCount);
    int height = random(5, maxHeight);
    int x = rightX + 20 + i * (barWidth + barSpacing);
    int y = baseY - height;
    tft.fillRect(x, y, barWidth, height, chassisColors[random(12)]);
    /*
    for (int i = 0; i < barCount; i++) {
      int height = random(5, maxHeight);
      int x = rightX + 20 + i * (barWidth + barSpacing);
      int y = baseY - height;
      tft.fillRect(x, y, barWidth, height, chassisColors[random(12)]);
    }*/

    // sprite.pushSprite(0, 35);
  }
}
void drawVisualizesmall(int rightW, int rightY, int rightX) {

  if (vzstatus) {
    int barCount = 15;  // ปรับจำนวนแท่ง
    int barSpacing = 5;
    int availableWidth = rightW - 40;
    int barWidth = (availableWidth - (barCount - 1) * barSpacing) / barCount;
    int maxHeight = 60;

    int baseY = rightY + 145;  // ฐาน visualizer ใกล้ด้านล่าง

    int i = random(barCount);
    int height = random(5, maxHeight);
    int x = rightX + 20 + i * (barWidth + barSpacing);
    int y = baseY - height;
    tft.fillRect(x, y, barWidth, height, chassisColors[random(12)]);

    /*
    for (int i = 0; i < barCount; i++) {
      int height = random(5, maxHeight);
      int x = rightX + 20 + i * (barWidth + barSpacing);
      int y = baseY - height;
      tft.fillRect(x, y, barWidth, height, chassisColors[random(12)]);
    }*/

    // sprite.pushSprite(0, 35);
  }
}
void updateVolumeBar() {
  int leftWidth = 90;
  int rightX = leftWidth + 5;
  int rightW = 320 - leftWidth - 5;
  int rightY = 0;

  tft.setTextDatum(TL_DATUM);
  tft.setTextColor(TFT_YELLOW, displayColor3);
  tft.drawString("Vol:", rightX + 10, rightY + 145, 1);

  int volBarWidth = map(VOLUME, 0, 30, 0, rightW - 50);
  tft.fillRect(rightX + 40, rightY + 145, volBarWidth, 8, TFT_GREEN);
  tft.drawRect(rightX + 40, rightY + 145, rightW - 50, 8, TFT_WHITE);
}