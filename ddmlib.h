/************************************************************************
*   SFP Optical Power Meter using ATMEGA328P
*   File:   ddmlib.h
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

#ifndef ddmlib_h
#define ddmlib_h
#include <Arduino.h>

class ddm
{
  public:
    float getRX();
    float getTX();
    float getTemp();
    float getVolts();
    void getALM(boolean & _temp_hi, boolean & _temp_low, boolean & _vcc_hi, 
                boolean & _vcc_low,boolean & _tx_hi, boolean & _tx_low,boolean
                & _rx_hi, boolean & _rx_low );
    void getWARN(boolean & _temp_hi_warn, boolean & _temp_low_warn, boolean
                 & _vcc_hi_warn, boolean & _vcc_low_warn,boolean & _tx_hi_warn, 
                 boolean & _tx_low_warn,boolean & _rx_hi_warn, boolean & _rx_low_warn );
    uint16_t calibrateRXpower();
    void getINFO();
    void debugDDM();    
    void DDM_All_Read(void);
    void Read_VendorDetails(void);
  private:
    uint8_t I2CReadEEPROM(const byte i2c_addr, unsigned int address);
};

#endif
