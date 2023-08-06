#include <SPI.h>
#include <mcp2515.h>
#include <Wire.h>
#include "Waveshare_LCD1602_RGB.h"
#include <string.h>
Waveshare_LCD1602_RGB lcd(16,2);  //16 characters and 2 lines of show
struct can_frame canMsg;
MCP2515 mcp2515(9);
void setup() {
  Serial.begin(115200);
  mcp2515.reset();
  mcp2515.setBitrate(CAN_500KBPS, MCP_16MHZ);
  mcp2515.setNormalMode();
  Serial.println("------- CAN Read ----------");
  Serial.println("ID  DLC   DATA");
  // initialize
  lcd.init();
  lcd.setRGB(200,200,200);
}
void loop() {
  if (mcp2515.readMessage(&canMsg) == MCP2515::ERROR_OK) {
    Serial.print(canMsg.can_id, HEX); // print ID
    Serial.print(" ");
    Serial.print(canMsg.can_dlc, HEX); // print DLC
    Serial.print(" ");
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
      Serial.print(canMsg.data[i],HEX);
      Serial.print(" ");
    }
    Serial.println();
    char print_data[canMsg.can_dlc][10];
    lcd.setCursor(0,0);
    lcd.send_string("This is RPM!");
    lcd.setCursor(0,1);
    for (int i = 0; i<canMsg.can_dlc; i++)  {  // print the data
      sprintf(print_data[i], "%d", canMsg.data[i]);
      lcd.send_string(print_data[i]);
    }
  }
}