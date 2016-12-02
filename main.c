/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include <project.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "Adafruit_SHT31.h"

#define false (0)
#define true (!false)

#define POLYNOMIAL (0x31)    //49

uint8_t Adafruit_SHT31_crc8(uint8_t *data, int len)
{
  uint8_t crc = 0xFF;
  int j;
    
  for ( j = len; j; --j ) {
    int i;
    
    crc ^= *data++;

    for ( i = 8; i; --i ) {
        crc = ( crc & 0x80 ) ? (crc << 1) ^ POLYNOMIAL : (crc << 1);
    }
  }
  return crc;
}

float Adafruit_SHT31_readTemperature(void) {
    while (! Adafruit_SHT31_readTempHum())
    {
        CyDelay(200); //was 200
    }

    return temp;
}
  

float Adafruit_SHT31_readHumidity(void) {
    while (! Adafruit_SHT31_readTempHum())
    {
        CyDelay(200); //was 200
    }

    return humidity;
}


int Adafruit_SHT31_readTempHum(void) {
    uint8_t readbuffer[6];

    Adafruit_SHT31_writeCommand(SHT31_MEAS_HIGHREP);  //0x2400
  
    CyDelay(500);  //was 500

    I2C_1_MasterReadBuf(_i2caddr, readbuffer, 6, I2C_1_MODE_COMPLETE_XFER);

    uint16_t ST, SRH;
    ST = readbuffer[0];
    ST <<= 8;
    ST |= readbuffer[1];

    if (readbuffer[2] != Adafruit_SHT31_crc8(readbuffer, 2)) 
        return false;

    SRH = readbuffer[3];
    SRH <<= 8;
    SRH |= readbuffer[4];

    if (readbuffer[5] != Adafruit_SHT31_crc8(readbuffer+3, 2)) 
        return false;

    // Serial.print("ST = "); Serial.println(ST);
    double stemp = ST;
    stemp *= 175;
    stemp /= 0xffff;
    stemp = -45 + stemp;
    temp = stemp;

    //  Serial.print("SRH = "); Serial.println(SRH);
    double shum = SRH;
    shum *= 100;
    shum /= 0xFFFF;

    humidity = shum;

    return true;
}

int Adafruit_SHT31_writeCommand(uint16_t cmd) {
    uint8 wrData[2];
    int   result;

    wrData[0] = (cmd >> 8) & 0xFF;  //0x30 or 0x24
    wrData[1] = cmd & 0xFF;         //0xA2 or 0x00
    
    while (I2C_1_MasterWriteBuf(_i2caddr, wrData, 2, I2C_1_MODE_COMPLETE_XFER))
    {
        CyDelay(200);  //was 200
    }

    result = true;
    
    return result;
}


void Adafruit_SHT31_reset(void) {
  Adafruit_SHT31_writeCommand(SHT31_SOFTRESET);   //0x30A2
  CyDelay(10);  //was 10
}


int Adafruit_SHT31_begin(uint8_t i2caddr) {
  _i2caddr = i2caddr;
  Adafruit_SHT31_reset();
  //return (readStatus() == 0x40);
  return true;
}

int main()
{
    int     i;
    char    str[64];
    int     dots = 0;

    CyGlobalIntEnable; /* Enable global interrupts. */

    I2C_1_Start();
    LCD_Char_1_Start() ;
    
    //while(1)
    Adafruit_SHT31_begin(0x44);
    
    for(;;)
    {
        float t;
//        float h;
        
        t = Adafruit_SHT31_readTemperature();
//        h = Adafruit_SHT31_readHumidity();

        sprintf( str, "Temp: %3dF ", (int) ((t * 9/5) + 32.5) );
        
        for (i = 0; i < dots; ++i)
            strcat( str, "." );
        for (i = 0; i < 5 - dots; ++i)
            strcat( str, " " );
        
        LCD_Char_1_Position(0, 0);
        LCD_Char_1_PrintString(str);

//        sprintf( str, "Hum:  %3d%%", (int) (h + 0.5) );
//        LCD_Char_1_Position(1, 0);
//        LCD_Char_1_PrintString(str);

        CyDelay(1000);
        dots = (dots + 1) % 6;
    }
}

/* [] END OF FILE */
