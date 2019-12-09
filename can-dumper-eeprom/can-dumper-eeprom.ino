#include <Arduino.h>
#include <SPI.h>
#include <avr/pgmspace.h>
#include "mcp_can.h"
#include "mcp_can_dfs.h"
#include <EEPROM.h>

const int SPI_CS_PIN = 10;

MCP_CAN CAN(SPI_CS_PIN);
unsigned char flagRecv = 0;
unsigned char len = 0;
unsigned char buf[8];
char str[20];

void attachCAN()
{
//  #if defined(__AVR_ATmega32U4__) // Arduino Pro Micro
   pinMode(7, INPUT);
   attachInterrupt(digitalPinToInterrupt(7), MCP2515_ISR, FALLING); // start interrupt
//  #else // Other Arduinos (Nano in my case)
//    pinMode(2, INPUT);
//    attachInterrupt(digitalPinToInterrupt(2), MCP2515_ISR, FALLING); // start interrupt
//  #endif
}

void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println("Init");
START_INIT:
  if(CAN_OK == CAN.begin(MCP_STDEXT, CAN_500KBPS, MCP_8MHZ)) {
    Serial.println(F("CAN ok!"));
  } else {
    Serial.println(F("CAN fail"));
    delay(100);
    goto START_INIT;
  }

  attachCAN();

  CAN.setMode(MCP_LISTENONLY);

  if ((EEPROM[1022] == 0xFF) && (EEPROM[1023] == 0xFF)) {
    EEPROM[1022] = 0x0;
    EEPROM[1023] = 0x0;
  }

}

void MCP2515_ISR()
{
    flagRecv = 1;
}

void loop()
{
    if(flagRecv)
    {
        flagRecv = 0;
        while (CAN_MSGAVAIL == CAN.checkReceive())
        {
            // read data,  len: data length, buf: data buf
            CAN.readMsgBuf(&len, buf);

            /*
            Serial.print(millis());
            Serial.print("\t");
            Serial.print(CAN.getCanId(),HEX);
            Serial.print("\t");
            for(int i = 0; i<len; i++)
            {
                Serial.print(buf[i],HEX);Serial.print("\t");
            }
            Serial.println();
            */

            if ((CAN.getCanId() == 0x7E8) &&
                (buf[0] == 0x05) &&
                (buf[1] == 0x62) &&
                (buf[2] == 0x20) &&
                (buf[3] >= 0x0A) &&
                (buf[3] <= 0x0D)
               ) {
              unsigned int eepromOffset = EEPROM[1022] | EEPROM[1023] << 8;
              if (eepromOffset > 999) eepromOffset = 0;
              
              Serial.print(eepromOffset);
              Serial.print(" ");
              Serial.print(buf[3], HEX);
              Serial.print(" ");
              Serial.print(buf[4], HEX);
              Serial.print(" ");
              Serial.print(buf[5], HEX);
              Serial.println();

              eepromOffset++;
              EEPROM.update(eepromOffset, buf[4]);
              eepromOffset++;
              EEPROM.update(eepromOffset, buf[5]);
              EEPROM.update(1022,  eepromOffset & 0x00FF);
              EEPROM.update(1023, (eepromOffset & 0xFF00) >> 8);


            }
/*            else
            {
                          Serial.print(millis());
            Serial.print("\t");
            Serial.print(CAN.getCanId(),HEX);
            Serial.print("\t");
            for(int i = 0; i<len; i++)
            {
                Serial.print(buf[i],HEX);Serial.print("\t");
            }
            Serial.println();

            }
*/
        }
    }
}
