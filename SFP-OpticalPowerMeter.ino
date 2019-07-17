/************************************************************************
*   SFP Optical Power Meter using ATMEGA328P
*   File:   SFP-OpticalPowerMeter.c
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

#include <SPI.h>
#include <Wire.h>
#include <math.h>

#include "ddmlib.h"
#include "catbin.h"

#define M1 0xAA 
#define M2 0xBB
#define M3 0xCC
#define M4 0xDD
#define M5 0xEE
#define M6 0xFF

ddm sfp;

const int TX_fault =  8;
const int LOS =  9;
const int TX_disable =  1;   // TX en, 0 = TX ensble ,1= disable ,

//const int TX_led =  A2;
const int RX_led = 13; // ON Board LED

boolean debug_flag = 0;
boolean TX_status = 0;
boolean RX_status = 0;
boolean V_status = 0;

float temp_t;
float volts;
float optical_tx;
float optical_rx;

boolean temp_hi, temp_low, vcc_hi, vcc_low, tx_hi, tx_low, rx_hi, rx_low;
boolean temp_hi_warn, temp_low_warn, vcc_hi_warn, vcc_low_warn, tx_hi_warn, tx_low_warn, rx_hi_warn, rx_low_warn;
int incomingByte = 0;
char do_once = 0;

void setup()   {

  pinMode(TX_disable, OUTPUT);        // TX en, GND = TX end,
  pinMode(TX_fault, INPUT_PULLUP);    // TxFault , 1 = OK, 0 = Fault
  pinMode(LOS, INPUT_PULLUP);         // LOS, 1 = OK, 0 = LOS
  //pinMode(TX_led, OUTPUT); 
  pinMode(RX_led, OUTPUT); 
   
  Wire.begin();  
  Serial.begin(9600);
    
  delay(1000);
  Serial.println("SFP Power Meter Ver 1.0");
  Serial.println(" ");
  digitalWrite(TX_disable, HIGH);    // Enable Laster TX
}

void loop() {

 while (1) {
  if (Serial.available() > 0) {
     incomingByte = Serial.read();
     Serial.write(13);
     do_once = 1;
  }

  if(do_once) {
  if (incomingByte == M1) {
      Serial.println("Show TX power");
      showTX();
      if(debug_flag) debug_flag = 0;
      } else if (incomingByte == M2) {
      Serial.println("Show RX power");
      showRX();
    } else if (incomingByte == M3) {
      Serial.println("Show TX-RX power");
      showTX();
      showRX();
      if(debug_flag) debug_flag = 0;
    } else if(incomingByte == M4) {
      Serial.println("SFP Module Details");
      sfp.Read_VendorDetails();
      sfp.getINFO();
      if(debug_flag) debug_flag = 0;
    } else if(incomingByte == M5) {
      Serial.println("System Details");
      showSystem();
      if(debug_flag) debug_flag = 0;
    } else if(incomingByte == M6) {
      Serial.println("DBG Enable");
      debug_flag = 1;
    } else {
      //Serial.println("Not Implemented");
      //if(debug_flag) debug_flag = 0;
    }
      do_once = 0;
  }
  if (debug_flag) {
    sfp.debugDDM();
    sfp.getALM(temp_hi, temp_low, vcc_hi, vcc_low, tx_hi, tx_low, rx_hi, rx_low);
    sfp.getWARN(temp_hi_warn, temp_low_warn, vcc_hi_warn, vcc_low_warn, tx_hi_warn,
                tx_low_warn, rx_hi_warn, rx_low_warn); 
    }
    checkALM();  
    delay(1000);
  }
}

void checkALM()
{
    if ( 30 > optical_rx  && optical_rx > -40) {  
      digitalWrite(RX_led, HIGH); 
      RX_status = 1;
    } else {
      digitalWrite(RX_led, LOW); 
      RX_status = 0;
    } 
}

void showTX()
{
  sfp.DDM_All_Read ();
  optical_tx = sfp.getTX();
  Serial.print("TX Power: ");
  Serial.print(optical_tx);
  Serial.println(" dbm");
}

void showRX()
{
  sfp.DDM_All_Read ();
  optical_rx = sfp.getRX();
  Serial.print("RX Power: ");
  Serial.print(optical_rx);
  Serial.println(" dbm");
}

// show status/alarm in bottom bar
void showStatus()
{
  if (!TX_status)
      Serial.print("EN");
  else
      Serial.print("NA");

  if (RX_status)
      Serial.print("OK ");
  else
    Serial.print("LOS"); 
}

void showSystem()
{
  sfp.DDM_All_Read ();
  volts = sfp.getVolts();
  temp_t = sfp.getTemp();
  optical_rx = sfp.getRX();
  optical_tx = sfp.getTX();

  Serial.print("Temp   :");
  Serial.println(temp_t);
  Serial.print("Volt   :");
  Serial.println(volts);
  Serial.print("TXPower:");
  Serial.println(optical_tx);
  Serial.print("RXPower:");
  Serial.println(optical_rx);
}
