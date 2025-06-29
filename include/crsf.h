#ifndef TURBOLINK_CRSF_H
#define TURBOLINK_CRSF_H

#include "Arduino.h"
#include "crsffront.h"

namespace TurboLinkCrsf {


    class Crsf {

    public:

        Crsf(HardwareSerial& serial) {
            serial.begin(CRSF_BAUDRATE);
            m_crsf.begin(serial);
        }

        void update() {
            
            const float rpm = 156000.0;    // 1/min
            const float fuelFlow = 350;    // ml/min
            const float fuelUsed = 2000.0; // ml
            const float dummy = 0.0;

            // Factors to scale to uint max
            m_crsf.sendTurbineData1(
                ((float)m_i/10.0)*rpm,      // rpm (frame: voltage)
                ((float)m_i/10.0)*fuelFlow, // fuel flow (frame: current)
                ((float)m_i/10.0)*fuelUsed, // fuel used (frame: consumption)
                ((float)m_i/10.0)*dummy     // nothing (frame: remaining)
            );


            m_i++;

            if (m_i > 10) {
                m_i = 0;
            }
        }
        
        
    private:
        CrsfFront::CrsfTelemetry m_crsf;

        int m_i{0};

    };



}


#endif