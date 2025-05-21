
int calcentervalue(int valuemin, int valuemax, bool adddeadzone) {
  int deadzone = valuemax - valuemin;
  int center = (valuemax + valuemin) / 2;
  int valuecenter = 0;
  if (adddeadzone)
    valuecenter = center + deadzone;
  else
    valuecenter = center - deadzone;
  return valuecenter;
}

int convertJoystickValues(int value, int valuemin, int valuemax, bool reverse) {
  int valuemaxs = valuemax;
  int valuemins = valuemin;
  if (value > valuemaxs) {
    value = map(value, valuemaxs, 4095, 0, MAXSPEED);
  } else if (value < valuemins) {
    value = map(value, valuemins, 0, 0, -MAXSPEED);
  } else {
    value = 0;
  }
  if (reverse) {
    value = -value;
  }
  return value;
}

const unsigned long HOLD_THRESHOLD = 1000;  // 3 วิ
unsigned long buttonDownTime = 0;
bool buttonHeld = false;

void eventbutton() {
  if (digitalRead(normalButtonPin) == HIGH) {
    if (buttonDownTime == 0) {
      buttonDownTime = millis();  // กดครั้งแรก
    } else if (!buttonHeld && millis() - buttonDownTime >= HOLD_THRESHOLD) {
      buttonHeld = true;
      if (MainmenuIndex != 0) {
        updateMenuStatus();
        MainmenuIndex = 0;
        resetvalue = true;
        carstop();
        playClickSound();
      }
    }
  } else {
    // reset state
    if (buttonDownTime > 0 && !buttonHeld) {
      buttonDownTime = 0;
      buttonHeld = false;
      playClickSound();
      // --- หน้าเมนู ---
      if (MainmenuIndex == 0) {
        if (indexmenu == 0 || indexmenu == 1) {
          if (WiFi.getMode() != WIFI_OFF)
            disconnectWiFi();
        } else if (indexmenu == 2) {
          if (WiFi.getMode() == WIFI_OFF)
            setupwifi();
          if (wsControl.isConnected())
            wsControl.sendTXT("G,1");
        } else if (indexmenu == 4) {
          if (WiFi.getMode() != WIFI_OFF)
            disconnectWiFi();
          else if (WiFi.getMode() == WIFI_OFF)
            setupwifi();
          resetvalue = true;
          return;
        }
        MainmenuIndex = indexmenu;
        resetvalue = true;
        return;
      }
      // --- หน้าบังคับ
      if (MainmenuIndex == 1) {
        //ส่วนนี้ทำงานแค่ครั้งเดียวเท่านั้น
        valuemax1_max = calcentervalue(minX1, maxX1, true);
        valuemax1_min = calcentervalue(minX1, maxX1, false);
        valuemay1_max = calcentervalue(minY1, maxY1, true);
        valuemay1_min = calcentervalue(minY1, maxY1, false);
        valuemax2_max = calcentervalue(minX2, maxX2, true);
        valuemax2_min = calcentervalue(minX2, maxX2, false);
        if (pbSwitch) {
          bannerinfo("ON");
          pbSwitch = false;
        } else {
          bannerinfo("OFF");
          pbSwitch = true;
        }
        cleantft = true;
        return;
      }

      // --- การจัดการปุ่มทั่วไป ---
      if (MainmenuIndex == 2) {
        if (mp3MenuIndex == 0) {
          if (wsControl.isConnected()) {
            wsControl.sendTXT("M," + String(selectedTrack));
            vzstatus = true;
            cleantft = true;
          }
          return;
        }
        if (mp3MenuIndex == 1) {
          if (wsControl.isConnected()) {
            wsControl.sendTXT("R,1");
            vzstatus = true;
            cleantft = true;
          }
          return;
        }
        if (mp3MenuIndex == 2) {
          if (wsControl.isConnected()) {
            wsControl.sendTXT("S,1");
            vzstatus = false;
            cleantft = true;
          }
          return;
        }
        if (mp3MenuIndex == 3) {
          MainmenuIndex = 0;
          resetvalue = true;
          return;
        }
        return;
      }

      // --- การจัดการปุ่มทั่วไป ---
      if (MainmenuIndex == 3) {
        minX1 = 9999;
        maxX1 = 0;
        minY1 = 9999;
        maxY1 = 0;
        minX2 = 9999;
        maxX2 = 0;
        return;
      }
    }
    buttonDownTime = 0;
    buttonHeld = false;
  }
}





/*
int convertJoystickValues(int value ,int speedmax,bool reverse) {
   int   minspeed = 4094-speedmax;
  if (value >= 2200) {
    value = map(value, 2200, 4095,0, speedmax);
  } else if (value <= 1800) {
    value = map(value, 1800, 0,0,-speedmax);
  } else {
    value = 0;  // จุดกลาง
  }
  if (reverse) {
    value = speedmax - value;
  }
  return value;
}
*/
/*
int convertJoystickValues(int value, bool reverse) {
 
    if (value >= 2200) {
    // Joystick pushed forward
    value = map(value, 2200, 4095, 127, MAXSPEED);
  } else if (value <= 1800) {
    // Joystick pulled back
    value = map(value, 1800, 0, 127, 0);
  } else {
    // Joystick in center
    value = 127;
  }

  // Check direction
  if (reverse) {
    value = 254 - value;
  }
  return value;
}*/



/*
    if (abs(rf_speed) > 1000) {
      if ((rf_speed > 0 && mtrRFpwmValue < 0) || (rf_speed < 0 && mtrRFpwmValue > 0))
        rf_speed = 0;
    } else
      rf_speed = 0;

    if (abs(lf_speed) > 1000) {
      if ((lf_speed > 0 && mtrLFpwmValue < 0) || (lf_speed < 0 && mtrLFpwmValue > 0))
        lf_speed = 0;
    } else
      lf_speed = 0;

    if (abs(rr_speed) > 1000) {
      if ((rr_speed > 0 && mtrRRpwmValue < 0) || (rr_speed < 0 && mtrRRpwmValue > 0))
        rr_speed = 0;
    } else
      rr_speed = 0;

    if (abs(lr_speed) > 1000) {
      if ((lr_speed > 0 && mtrLRpwmValue < 0) || (lr_speed < 0 && mtrLRpwmValue > 0))
        lr_speed = 0;
    } else
      lr_speed = 0;*/


/*
   
      if ((rf_speed > 0 && mtrRFpwmValue < 0) || (rf_speed < 0 && mtrRFpwmValue > 0))
        rf_speed = 0;

      if ((lf_speed > 0 && mtrLFpwmValue < 0) || (lf_speed < 0 && mtrLFpwmValue > 0))
        lf_speed = 0;

      if ((rr_speed > 0 && mtrRRpwmValue < 0) || (rr_speed < 0 && mtrRRpwmValue > 0))
        rr_speed = 0;

      if ((lr_speed > 0 && mtrLRpwmValue < 0) || (lr_speed < 0 && mtrLRpwmValue > 0))
        lr_speed = 0;
      */
/*สูตรใหม่

      /*
      สูตรหาค่า max_w
      max_RPM = 319;
      max_w = (319 * 2 * PI) / 60
       ≈ (319 * 6.283) / 60
       ≈ 2004.18 / 60
       ≈ 33.4 rad / s
*/
/*
      rf_speed = constrain(joyyaxis + joyxaxis, -MAXSPEED, MAXSPEED);
      lf_speed = constrain(joyyaxis - joyxaxis, -MAXSPEED, MAXSPEED);
      rr_speed = constrain(joyyaxis - joyxaxis, -MAXSPEED, MAXSPEED);
      lr_speed = constrain(joyyaxis + joyxaxis, -MAXSPEED, MAXSPEED);
    
      //  joyxaxis = convertJoystickValues(analogRead(JOY_L_X_PIN), false);
      //   joyyaxis = convertJoystickValues(analogRead(JOY_L_Y_PIN), false);
      float max_vel_linear = 1.0;   // เมตร/วินาที
      float max_vel_angular = 2.0;  // เรเดียน/วินาที

      float joyLeftY_norm = convertJoystickValues(analogRead(JOY_L_Y_PIN), false);
      float joyLeftX_norm = convertJoystickValues(analogRead(JOY_L_X_PIN), true);  // สังเกตการกลับด้านแกน X (ถ้าจำเป็น)
      float joyRightX_norm = convertJoystickValues(analogRead(JOY_R_X_PIN), true);

      // กำหนด Vx, Vy, Wz จากค่าจอยสติ๊ก
      float Vx = joyLeftY_norm * max_vel_linear;
      float Vy = joyLeftX_norm * max_vel_linear;
      float Wz = joyRightX_norm * max_vel_angular;

      // คำนวณความเร็วเชิงมุมของล้อแต่ละล้อ (ใช้สูตร Inverse Kinematics)
      float R = 0.04;   // รัศมีล้อ (เมตร)
      float L = 0.14;   // ครึ่งความยาวฐานล้อ (เมตร)
      float W = 0.095;  // ครึ่งความกว้างฐานล้อ (เมตร)

      float w_fl = (1 / R) * (Vx + Vy - L * Wz - W * Wz);
      float w_fr = (1 / R) * (Vx - Vy + L * Wz - W * Wz);
      float w_rl = (1 / R) * (Vx - Vy - L * Wz + W * Wz);
      float w_rr = (1 / R) * (Vx + Vy + L * Wz + W * Wz);

      // แปลงความเร็วเชิงมุม (w) เป็นความเร็ว PWM (motorSpeed)
      // ... (ต้องมีการปรับสเกลให้เหมาะสมกับมอเตอร์และ PWM)

      float max_w = 33.4;  // ความเร็วเชิงมุมสูงสุดของล้อ (เรเดียนต่อวินาที)

      int motorSpeed_fl = map(w_fl, -max_w, max_w, -MAXSPEED, MAXSPEED);  // max_w คือความเร็วเชิงมุมสูงสุดที่มอเตอร์ทำได้
      int motorSpeed_fr = map(w_fr, -max_w, max_w, -MAXSPEED, MAXSPEED);
      int motorSpeed_rl = map(w_rl, -max_w, max_w, -MAXSPEED, MAXSPEED);
      int motorSpeed_rr = map(w_rr, -max_w, max_w, -MAXSPEED, MAXSPEED);

      rf_speed = motorSpeed_fr;
      lf_speed = motorSpeed_fl;
      rr_speed = motorSpeed_rr;
      lr_speed = motorSpeed_rl;


*/