/************************************************************************
*   SFP Optical Power Meter using ATMEGA328P
*   File:   ddmlib.c
*   Author:  Jithin Krishnan.K
*       Rev. 1.0 : 17/07/2019 :  02:34 PM
* 
* This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*   
* SPI DDM interface is based on the works of 
* SFPDDM (Arduino library for interfacing SFP modules and reading DDM 
* information as per SFF-8472)
* https://github.com/SloMusti/sfpddm
*
*********************************************************************/

#include <Wire.h>
#include <math.h>
#include "ddmlib.h"

const byte EEPROM_ID_VENDOR = 0x50;
const byte EEPROM_ID_DDM    = 0x51;

byte DDM[256], VENDOR[256]; 

float _temp_t;
float _volts;
float _optical_tx;
float _optical_rx;

// get RX power
float ddm::getRX()
{
   float _optical_rx2 = (DDM[104]<<8 | DDM[105]) * 0.0001;
   _optical_rx = 10 * log10((float)(DDM[104]<<8 | DDM[105]) * 0.0001);
   return(_optical_rx);
}

// get TX power
float ddm::getTX()
{
   float _optical_tx1 = (DDM[102]<<8 | DDM[103]) ;   
   float _optical_tx2 =  _optical_tx1 * 0.0001;
   _optical_tx = 10 * log10(_optical_tx2);
   return(_optical_tx);
}

// get voltages
float ddm::getTemp()
{
   _temp_t = DDM[96] + (float) DDM[97]/256;
    return(_temp_t);
}

// get voltages
float ddm::getVolts()
{
   _volts = (DDM[98]<<8 |  DDM[99]) * 0.0001;
   return(_volts);
}

// check warning status
void ddm::getALM(boolean & _temp_hi, boolean & _temp_low, boolean & 
    _vcc_hi, boolean & _vcc_low,boolean & _tx_hi, boolean & _tx_low,boolean & _rx_hi, boolean & _rx_low )
{
  _temp_hi = bitRead(DDM[112], 7);
  _temp_low = bitRead(DDM[112], 6); 
  _vcc_hi = bitRead(DDM[112], 5);
  _vcc_low = bitRead(DDM[112], 4);
  _tx_hi = bitRead(DDM[112], 1);
  _tx_low = bitRead(DDM[112], 0);
  _rx_hi = bitRead(DDM[113], 7);
  _rx_low = bitRead(DDM[113], 6);
  
  Serial.print("TEMP_HI: ");
  Serial.println(_temp_hi);
  Serial.print("VCC_LOW: ");
  Serial.println(_vcc_low);
  Serial.print("TX_LOW: ");
  Serial.println(_tx_low);  
  Serial.print("RX_LOW: ");
  Serial.println(_rx_low);
}

// get warning bits
void ddm::getWARN(boolean & _temp_hi_warn, boolean & _temp_low_warn, boolean & _vcc_hi_warn,
     boolean & _vcc_low_warn,boolean & _tx_hi_warn, boolean & _tx_low_warn,boolean & _rx_hi_warn, boolean & _rx_low_warn )
{
  _temp_hi_warn = bitRead(DDM[116], 7);
  _temp_low_warn = bitRead(DDM[116], 6); 
  _vcc_hi_warn = bitRead(DDM[116], 5);
  _vcc_low_warn = bitRead(DDM[116], 4);
  _tx_hi_warn = bitRead(DDM[116], 1);
  _tx_low_warn = bitRead(DDM[116], 0);
  _rx_hi_warn = bitRead(DDM[117], 7);
  _rx_low_warn = bitRead(DDM[117], 6);

  Serial.println("");
  Serial.print("TEMP_HI_WARN: ");
  Serial.println(_temp_hi_warn);
  Serial.print("VCC_LOW_WARN: ");
  Serial.println(_vcc_low_warn);
  Serial.print("TX_LOW_WARN: ");
  Serial.println(_tx_low_warn);  
  Serial.print("RX_LOW_WARN: ");
  Serial.println(_rx_low_warn);
}

// calibrate RX power
uint16_t ddm::calibrateRXpower()
{
  float temp = 0;
  int _optical_rx_raw = (DDM[104]<<8 | DDM[105]);
  temp = (DDM[56]<<24 | DDM[57]<<16 | DDM[58]<<8 | DDM[59]) * _optical_rx_raw;
  temp += (DDM[60]<<24 | DDM[61]<<16 | DDM[62]<<8 | DDM[63]) * _optical_rx_raw;
  temp += (DDM[64]<<24 | DDM[65]<<16 | DDM[66]<<8 | DDM[67]) * _optical_rx_raw;
  temp += (DDM[68]<<24 | DDM[69]<<16 | DDM[70]<<8 | DDM[71]) * _optical_rx_raw;
  temp += (DDM[72]<<24 | DDM[73]<<16 | DDM[74]<<8 | DDM[75]) ; //offset
  Serial.print("RXcalibrated: ");
  Serial.println(temp);
  return (int16_t)temp;
}

// get infod
void ddm::getINFO()
{
     int i, j = 0;
     char INFO[16];
     
     Serial.print("SFP Make: ");
     for (int i = 20, j = 0; i <= 35; i++, j++)
          INFO[j] = VENDOR[i];
          
     for (j = 0;j< 17;j++)
          Serial.print(INFO[j]);
          Serial.println(" ");
      
     Serial.print("Serial No: ");
     for (int i = 68, j = 0; i <= 83; i++, j++)
          INFO[j] = VENDOR[i];
          
     for (j = 0;j< 17;j++)
          Serial.print(INFO[j]);
     Serial.println(" ");
     Serial.print("Wave Length: ");     
     Serial.print(((VENDOR[60] & 0xFF) << 8) | (VENDOR[61] & 0xFF));
     Serial.println(" nm");
}

// Debug message/data
void ddm::debugDDM()
{
     Serial.print("DDM Calbrition Data 56:92 (HEX)> ");
     for (int i = 56; i <= 92; i++) {
        Serial.print(DDM[i]);
        Serial.print(" ");
     }
     Serial.println(" ");
     Serial.print("DDM Data 96:105 (HEX)> ");
     for (int i = 96; i < 106; i++) {
          Serial.print(DDM[i]);
          Serial.print(" ");
     }
     Serial.println(" ");
     Serial.print("DDM Data 96:105 (BIN)> ");
     for (int i = 96; i < 106; i++) {
        Serial.print(DDM[i], BIN);
        Serial.print(" ");
     }
     Serial.println(" ");
     
     int _optical_rx_raw = (DDM[104]<<8 | DDM[105]);
     Serial.print("_optical_rx_raw : ");
     Serial.println(_optical_rx_raw, HEX);
     
     Serial.println("");
     Serial.print("TX Power: ");
     Serial.println(_optical_tx);
     Serial.print(" dbm");
     Serial.print("RX Power: ");
     Serial.println(_optical_rx);
     Serial.print(" dbm");
     Serial.println(" ");    
}

// DDM Alarms and Controls Registers
void ddm::DDM_All_Read (void)
{
   for (int i = 0; i <256; i++)
  {
    DDM[i] = I2CReadEEPROM(EEPROM_ID_DDM, i);
  }
}
// Vendor Related Details
void ddm::Read_VendorDetails (void)
{
   for (int i = 0; i <256; i++)
  {
    VENDOR[i] = I2CReadEEPROM(EEPROM_ID_VENDOR, i);
  }
}

// Private Methods /////////////////////////////////////////////////////////////
// Functions only available to other functions in this library

// DDM Read Registers
uint8_t ddm::I2CReadEEPROM (const byte i2c_addr, unsigned int address)
{
  byte data;
  Wire.beginTransmission(i2c_addr);
  Wire.write(address);
  Wire.endTransmission();
  Wire.requestFrom(i2c_addr,(byte)1);
  while(Wire.available() == 0); 
    data = Wire.read();
    return data;
}
