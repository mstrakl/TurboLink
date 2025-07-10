#include <Arduino.h>

// https://www.rcgroups.com/forums/showthread.php?2245978-FrSky-S-Port-telemetry-library-easy-to-use-and-configurable


#define POLLING_ENABLED

#include "FrSkySportSensor.h"
#include "FrSkySportSensorAss.h"
#include "FrSkySportSensorEsc.h"
#include "FrSkySportSensorFcs.h"
#include "FrSkySportSensorFlvss.h"
#include "FrSkySportSensorGasSuite.h"
#include "FrSkySportSensorGps.h"
#include "FrSkySportSensorRpm.h"
#include "FrSkySportSensorSp2uart.h"
#include "FrSkySportSensorVario.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportDecoder.h"
#include "SoftwareSerial.h"

FrSkySportSensorAss ass;                               // Create ASS sensor with default ID
FrSkySportSensorEsc esc;                               // Create ESC sensor with default ID
FrSkySportSensorFcs fcs;                               // Create FCS-40A sensor with default ID (use ID8 for FCS-150A)
FrSkySportSensorFlvss flvss1;                          // Create FLVSS sensor with default ID
FrSkySportSensorFlvss flvss2(FrSkySportSensor::ID15);  // Create FLVSS sensor with given ID
FrSkySportSensorGasSuite gas;                          // Create GasSuite sensor with default ID
FrSkySportSensorGps gps;                               // Create GPS sensor with default ID
FrSkySportSensorRpm rpm;                               // Create RPM sensor with default ID
FrSkySportSensorSp2uart sp2uart;                       // Create SP2UART Type B sensor with default ID
FrSkySportSensorVario vario;                           // Create Variometer sensor with default ID


#ifdef POLLING_ENABLED
  #include "FrSkySportPollingSimple.h"
  FrSkySportDecoder decoder(new FrSkySportPollingSimple()); // Create telemetry object with simple polling
#else
  FrSkySportDecoder decoder;                           // Create decoder object without polling
#endif

uint32_t currentTime, displayTime;
uint16_t decodeResult;

#include "crsf.h"
TurboLinkCrsf::Crsf* Crsf = nullptr;
unsigned long crsfLastSend = 0;



#define DEBUG_FAKE_VALUES false

void dbgPrint();

void setup()
{
  // Configure the decoder serial port and sensors (remember to use & to specify a pointer to sensor)
  decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_10, &ass, &esc, &fcs, &flvss1, &flvss2, &gas, &gps, &rpm, &sp2uart, &vario);
  Serial.begin(420000);


  Crsf = new TurboLinkCrsf::Crsf(Serial);


}

void loop()
{

  if ( ! DEBUG_FAKE_VALUES ) {

    // Read and decode the telemetry data, note that the data will only be decoded for sensors
    // that that have been passed to the begin method. Print the AppID of the decoded data.
    decodeResult = decoder.decode();

    if (decodeResult == RPM_T1_DATA_ID || decodeResult == RPM_ROT_DATA_ID) {
      const uint32_t rxRpm = rpm.getRpm() * 2;
      const int32_t rxEgt = rpm.getT1();

      Crsf->update(rxRpm, rxEgt);

    } 

  } else {


    const uint32_t rxRpm = 123000;
    const int32_t rxEgt = 456;

    Crsf->update(rxRpm, rxEgt);

    delay(250);


  }




  //if(decodeResult != SENSOR_NO_DATA_ID) { 
  //  Serial.print("At " + String(millis())); Serial.print(" Decoded AppID 0x"); Serial.println(decodeResult, HEX); }
  //}
    
 
  //dbgPrint();




  //if (millis() - crsfLastSend > 250) {
  //  Crsf->update(rxRpm, rxEgt);
  //  crsfLastSend = millis();
  //}




}

void dbgPrint() {

 
  // Display data once a second to not interfeere with data decoding
  currentTime = millis();

  if(currentTime > displayTime)
  {
    displayTime = currentTime + 1000;

//    Serial.println("");
//
//    // Get airspeed sensor (ASS) data
//    Serial.print("ASS: airspeed = "); Serial.println(ass.getSpeed()); // Airspeed in km/h
//  
//    // Get ESC sensor data
//    Serial.print("ESC: voltage = "); Serial.print(esc.getVoltage());                      // ESC voltage in volts
//    Serial.print("V, current = "); Serial.print(esc.getCurrent());                        // ESC current draw in amps
//    Serial.print("A, rpm = "); Serial.print(esc.getRpm());                                // ESC motor rotations per minute
//    Serial.print(", consumption = "); Serial.print(esc.getConsumption());                 // ESC current consumtion in mAh
//    Serial.print("mAh, temp = "); Serial.print(esc.getTemp());                            // ESC temperature in degrees Celsius (can be negative, will be rounded)
//    Serial.print(" deg. C, SBEC voltage = "); Serial.print(esc.getSbecVoltage());         // ESC SBEC voltage in volts
//    Serial.print("V, SBEC current = "); Serial.print(esc.getSbecCurrent()); Serial.println("A");   // ESC SBEC current draw in amps
//
//    // Get current/voltage sensor (FCS) data
//    Serial.print("FCS: current = "); Serial.print(fcs.getCurrent());                    // Current consumption in amps
//    Serial.print("A, voltage = "); Serial.print(fcs.getVoltage()); Serial.println("V"); // Battery voltage in volts
//  
//    // Get LiPo voltage sensor (FLVSS) data (each cell1-cell6 voltage in volts)
//    Serial.print("FLVSS1: cell1 = "); Serial.print(flvss1.getCell1()); Serial.print("V, cell2 = "); Serial.print(flvss1.getCell2());
//    Serial.print("V, cell3 = "); Serial.print(flvss1.getCell3()); Serial.print("V, cell4 = "); Serial.print(flvss1.getCell4());
//    Serial.print("V, cell5 = "); Serial.print(flvss1.getCell5()); Serial.print("V, cell6 = "); Serial.print(flvss1.getCell6()); Serial.println("V"); 
//    Serial.print("FLVSS2: cell1 = "); Serial.print(flvss2.getCell1()); Serial.print("V, cell2 = "); Serial.print(flvss2.getCell2());
//    Serial.print("V, cell3 = "); Serial.print(flvss2.getCell3()); Serial.print("V, cell4 = "); Serial.print(flvss2.getCell4());
//    Serial.print("V, cell5 = "); Serial.print(flvss2.getCell5()); Serial.print("V, cell6 = "); Serial.print(flvss2.getCell6()); Serial.println("V"); 
//  
//    // Get Gas Suite sensor data
//    Serial.print("GAS: T1 = "); Serial.print(gas.getT1());                                         // Temperature #1 in degrees Celsuis (can be negative, will be rounded)
//    Serial.print(" deg. C, T2 = "); Serial.print(gas.getT2());                                     // Temperature #2 in degrees Celsuis (can be negative, will be rounded)
//    Serial.print(" deg. C, rpm = "); Serial.print(gas.getRpm());                                   // Rotations per minure
//    Serial.print(", res. volume = "); Serial.print(gas.getResVol());                               // Residual volume in ml
//    Serial.print("ml, res. percent = "); Serial.print(gas.getResPercent());                        // Residual percentage (0-100)
//    Serial.print("%, flow = "); Serial.print(gas.getFlow());                                       // Flow in ml/min
//    Serial.print("ml/min, max flow = "); Serial.print(gas.getFlowMax());                           // Maximum flow in ml/min
//    Serial.print("ml/min, avg flow = "); Serial.print(gas.getFlowAvg()); Serial.println("ml/min"); // Average flow in ml/min
//
//    // Get GPS sensor data
//    Serial.print("GPS: lat = "); Serial.print(gps.getLat(), 6); Serial.print(", lon = "); Serial.print(gps.getLon(), 6); // Latitude and longitude in degrees decimal (positive for N/E, negative for S/W)
//    Serial.print(", altitude = "); Serial.print(gps.getAltitude()); // Altitude in m (can be negative)
//    Serial.print("m, speed = "); Serial.print(gps.getSpeed());      // Speed in m/s
//    Serial.print("m/s, COG = "); Serial.print(gps.getCog());        // Course over ground in degrees (0-359, 0 = north)
//    char dateTimeStr[18]; 
//    sprintf(dateTimeStr, "%02u-%02u-%04u %02u:%02u:%02u", gps.getDay(), gps.getMonth(), gps.getYear() + 2000, gps.getHour(), gps.getMinute(), gps.getSecond());
//    Serial.print(", date/time = "); Serial.println(dateTimeStr); // Date (year - need to add 2000, month, day) and time (hour, minute, second)
//  
//    // Get RPM/temperature sensor data
    Serial.print("RPM: rpm = "); Serial.print(rpm.getRpm());                              // Rotations per minute
    Serial.print(", T1 = "); Serial.print(rpm.getT1());                                   // Temperature #1 in degrees Celsuis (can be negative, will be rounded)
    Serial.print(" deg. C, T2 = "); Serial.print(rpm.getT2()); Serial.println(" deg. C"); // Temperature #2 in degrees Celsuis (can be negative, will be rounded)
//  
//    // Get SP2UART sensor data (ADC3 and ADC4 voltage in V)
//    Serial.print("SP2UART: ADC3 = "); Serial.print(sp2uart.getAdc3());                // ADC3 voltage in volts
//    Serial.print("V, ADC4 = "); Serial.print(sp2uart.getAdc4()); Serial.println("V"); // ADC4 voltage in volts
//  
//    // Get variometer data (altitude in m, VSI in m/s)
//    Serial.print("VARIO: altitude = "); Serial.print(vario.getAltitude());          // Altitude in meters (can be negative)
//    Serial.print("m, VSI = "); Serial.print(vario.getVsi()); Serial.println("m/s"); // Vertical speed in m/s (positive - up, negative - down)
//
    Serial.println("");
  }
}

