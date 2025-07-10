#ifndef SCORPIOTEL_CRSF_FRONT_H
#define SCORPIOTEL_CRSF_FRONT_H

#include <AlfredoCRSF.h>



namespace CrsfFront
{


    void printBatteryBytes(const crsf_sensor_battery_t *batt) {
        const uint8_t *bytes = (const uint8_t *)batt;
        for (int i = 0; i < sizeof(crsf_sensor_battery_t); ++i) {
            Serial.print(bytes[i], HEX);
            Serial.print(" ");
        }
        Serial.println();
    }

    typedef struct crsf_sensor_battery_s2
    {
        unsigned voltage : 16;  // V * 10 big endian
        unsigned current : 16;  // A * 10 big endian
        unsigned cap1 : 8;
        //unsigned cap2 : 8;
        //unsigned cap3 : 8;
        unsigned capacity : 16; // mah big endian
        unsigned remaining : 8; // %
    } PACKED crsf_sensor_battery_t2;


    class CrsfTelemetry
    {

    public:
        CrsfTelemetry() = default;

        void begin(HardwareSerial &serial)
        {
            m_crsf.begin(serial);
        }

        // Via battery packet
        void sendTurbineData1(const uint16_t &rpm, const uint16_t &egt, const uint16_t &fuelUsed) {
            crsf_sensor_battery_t2 crsfBatt = {0};

            // Values are MSB first (BigEndian)
            crsfBatt.voltage = htobe16(rpm*10); 
            crsfBatt.current = htobe16(egt*10); 
            crsfBatt.capacity = htobe16(fuelUsed);

            float remaining = (1700.0f - min((float)fuelUsed, 1700.0f)) / 1700.0f;
            crsfBatt.remaining = (uint8_t) (100.0f * remaining);
            m_crsf.queuePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_BATTERY_SENSOR, &crsfBatt, sizeof(crsfBatt));

            //delay(50);
//
            //uint16_t fuelUsed2 = 950;
//
            //crsf_sensor_attitude_t crsfAttitude = {0};
            //crsfAttitude.pitch = htobe16(fuelUsed2*10);
            //crsfAttitude.roll = htobe16(0);
            //crsfAttitude.yaw = htobe16(0);
            //m_crsf.queuePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_ATTITUDE, &crsfAttitude, sizeof(crsfAttitude));
        }



//        void sendEscData(const float &voltage, const float &current, const float &capacity, const float &remaining)
//        {
//            crsf_sensor_battery_t crsfBatt = {0};
//
//            // Values are MSB first (BigEndian)
//            crsfBatt.voltage = htobe16((uint16_t)(voltage * 10.0)); // Volts
//            crsfBatt.current = htobe16((uint16_t)(current * 10.0)); // Amps
//            crsfBatt.capacity = htobe16((uint16_t)(capacity)) << 8; // mAh (with this implemetation max capacity is 65535mAh)
//            crsfBatt.remaining = (uint8_t)(remaining);
//
//            m_crsf.queuePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_BATTERY_SENSOR, &crsfBatt, sizeof(crsfBatt));
//        }

        // we are using roll-pitch-yaw packet for temp, currently
        // temperature is sent as T[degC] / 100
        // so 27deg is 0.27
        //
        void sendEscTemperature(const float &temp, const float alive)
        {

            crsf_sensor_attitude_t crsfAttitude = {0};

            Serial.println("Temp=" +String(temp));


            // This scales temp, so that when converted in edge tx
            // to angle degrees, it shows temperature in degC
            const float temp_to_milirad = temp * 174.53;

            // Values are MSB first (BigEndian)
            crsfAttitude.pitch = htobe16((uint16_t)(temp_to_milirad));
            crsfAttitude.roll = htobe16(0);
            crsfAttitude.yaw = htobe16(0);

            m_crsf.queuePacket(CRSF_SYNC_BYTE, CRSF_FRAMETYPE_ATTITUDE, &crsfAttitude, sizeof(crsfAttitude));
        }

        void readChannels() {
            m_crsf.update();
            m_ch14 = m_crsf.getChannel(14);
        }


        const int getReleaseCh14() {
            return m_ch14;
        }

    private:
        AlfredoCRSF m_crsf;

        int m_ch14{1500};
    };

}

#endif