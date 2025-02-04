/*
  This file is part of the Arduino_MKRIoTCarrier library.
  Copyright (c) 2020 Arduino SA.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/


#include <Arduino_MKRIoTCarrier.h>

//Touch pads values for using the case or just directly on the board
//Define on the sketch to use it
bool CARRIER_CASE = false;
    
MKRIoTCarrier::MKRIoTCarrier() {
}

int MKRIoTCarrier::_revision = 0;

int MKRIoTCarrier::begin() {

  pinMode(AREF_PIN,INPUT_PULLUP);
  MKRIoTCarrier::_revision = digitalRead(AREF_PIN);

  if (!CARRIER_CASE) {
    Buttons.updateConfig(200);
  }

  //Display
  if (_revision == BOARD_REVISION_2){
    Adafruit_ST7789 _display = Adafruit_ST7789(TFT_CS, TFT_DC, -1);
    display = _display;
  } else {
    Adafruit_ST7789 _display = Adafruit_ST7789(TFT_CS_REV1, TFT_DC_REV1, -1);
    display = _display;;
  }

  display.init(240, 240);//.begin(true);      // Initialize ST7789 screen
  pinMode(3,INPUT_PULLUP);     // RESET fix

  //Default rotation to align it with the carrier
  display.setRotation(2);
  display.fillScreen(ST77XX_BLACK);

  Buttons.begin(_revision == BOARD_REVISION_2);    //init buttons

  //init LEDs
  leds.begin();
  leds.clear();
  leds.show();

  //PMIC init
  PMIC.begin();
  PMIC.enableBoostMode();
  
  Relay1.begin();
  Relay2.begin();

  //Sensors
  uint8_t  sensorsOK = !Light.begin() << 0 |  !Pressure.begin() << 1 | !IMUmodule.begin() << 2  | !Env.begin() << 3;
  

  //If some of the sensors are not connected
  if(sensorsOK > 0 ){
    Serial.println("Error detected!");
    if(sensorsOK & 0b0001){
      Serial.println("Ambient light sensor is not connected!");
    }
    if(sensorsOK & 0b0010){
      Serial.println("Pressure sensor is not connected!");
    }
    if(sensorsOK & 0b0100){
      Serial.println("IMU is not connected");
    }
    if (MKRIoTCarrier::_revision != BOARD_REVISION_2) {
      if(sensorsOK & 0b1000){
        Serial.println("Environmental sensor is not connected!");
      }
    }
    return false;
  }
  
  if(!SD.begin(SD_CS)) {
    Serial.println("Sd card not detected");
  }

  return true;
}

int MKRIoTCarrier::getBoardRevision() {
  return MKRIoTCarrier::_revision;
}
