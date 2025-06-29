#ifndef TURBOLINK_CRSF_H
#define TURBOLINK_CRSF_H

#include "Arduino.h"
#include "crsffront.h"
#include "crsfmath.h"

namespace TurboLinkCrsf {

    const Point fuelTableGramPerMinute[] = {
        {  50,  50 },
        {  60,  60 },
        {  70,  65 },
        {  80,  70 },
        {  90,  90 },
        { 100, 100 },
        { 110, 120 },
        { 120, 140 },
        { 130, 160 },
        { 140, 170 },
        { 150, 210 },
        { 156, 250 }
    };


    class Crsf {

    public:

        Crsf(HardwareSerial& serial) {
            serial.begin(CRSF_BAUDRATE);
            m_crsf.begin(serial);
        }

        const float fuelFlow(const uint32_t rpm) {

            return 0.0;
        }

        void update(const uint32_t rpm, const int32_t egt) {

            const unsigned int dt = millis() - m_timeLastUpdate;
            m_timeLastUpdate = millis();
            
            const uint16_t rpm_div_1k = 0.001 * rpm;

            // Fuel flow approximation, in g/min
            const float fuelFlowGram = interpolateFromTable(
                rpm_div_1k,     
                fuelTableGramPerMinute, 
                sizeof(fuelTableGramPerMinute) / sizeof(Point)
            );

            // Convert to ml/min, assuming 0.8g/ml density
            // the factor is 1.25; The real factor is adjusted
            // to actual fuel used
            const float fuelFLow = fuelFlowGram * 1.35;

            // Integrate fuel used
            m_fuelUsed += fuelFLow * dt;

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