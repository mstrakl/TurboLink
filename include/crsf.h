#ifndef TURBOLINK_CRSF_H
#define TURBOLINK_CRSF_H

#include "Arduino.h"
#include "crsffront.h"
#include "crsfmath.h"

namespace TurboLinkCrsf {

    const Point fuelTableGramPerMinute[] = {
        {  500,  50 },
        {  600,  60 },
        {  700,  65 },
        {  800,  70 },
        {  900,  90 },
        { 1000, 100 },
        { 1100, 120 },
        { 1200, 140 },
        { 1300, 160 },
        { 1400, 170 },
        { 1500, 210 },
        { 1560, 250 }
    };


    class Crsf {

    public:

        Crsf(HardwareSerial& serial) {
            serial.begin(CRSF_BAUDRATE);
            m_crsf.begin(serial);
        }


        void update(const uint32_t rpm, const int32_t egt) {

            const unsigned int dt = millis() - m_timeLastUpdate;
            m_timeLastUpdate = millis();

            if (rpm > 45000) {

                // Fuel flow approximation, in g/min
                const float fuelFlowGram = interpolateFromTable(
                    (uint16_t)(rpm * 0.01),     
                    fuelTableGramPerMinute, 
                    sizeof(fuelTableGramPerMinute) / sizeof(Point)
                );

                // Convert to ml/min, assuming 0.8g/ml density
                // the factor is 1.25; The real factor is adjusted
                // to actual fuel used
                const float fuelFLow = fuelFlowGram * 1.35;

                // Integrate fuel used
                m_fuelUsed += fuelFLow * dt;

            } else {

                m_fuelUsed = 0; // Reset if engine stopped

            }

            const uint16_t rpm_div_1k = 0.001 * rpm;

            // Factors to scale to uint max
            m_crsf.sendTurbineData1(
                (uint16_t)rpm_div_1k*10,  // received as rpm/1000
                (uint16_t)egt*10, // received as degC
                (uint16_t)m_fuelUsed*0.001 // fuel used in ml
            );

        }
        
        
    private:
        CrsfFront::CrsfTelemetry m_crsf;

        unsigned long m_timeLastUpdate{0};
        unsigned long m_fuelUsed{0};




    };



}


#endif