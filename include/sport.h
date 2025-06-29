#ifndef SPORT_LIB_H
#define SPORT_LIB_H

#include <FrskySP.h>
#include <string.h>

// Help: 
// https://github.com/marhar/FrSkySportTelemetry/blob/master/FrSkySportSensorFlvss.cpp
// https://github.com/yaapu/FrskyTelemetryScript/wiki/FrSky-SPort-protocol-specs


//#define NUM_SENSORS 28
//uint8_t m_SensorIds[NUM_SENSORS] = {0x00,0xA1,0x22,0x83,0xE4,
//    0x45,0xC6,0x67,0x48,0xE9,
//    0x6A,0xCB,0xAC,0x0D,0x8E,
//    0x2F,0xD0,0x71,0xF2,0x53,
//    0x34,0x95,0x16,0xB7,0x98,
//    0x39,0xBA,0x1B};


// Packet structure
// 1	type 0x10,0x30,0x31,0x32
// 2	sensor logical ID
// 4	value
// 1	CRC



namespace Frsky {

    #pragma pack(push, 1)
    struct FrskySensorPacket {
        uint8_t type;
        uint8_t id_1;
        uint8_t id_2;
        uint8_t data[4];
        uint8_t crc;
    };
    #pragma pack(pop)

    class SPort {

    public:

        SPort(const int sportPin)
            : m_sport(sportPin)
        {}


        void update() {


            readLipoCells();




        }

        void readLipoCells() {

            static constexpr int ID = 0xA1;

            m_sport.write (0x7E);
            m_sport.write (ID);

            delay (11);  // wait for 11ms

            FrskySensorPacket packet = readPacket();

            //Serial.print("Data: ");
            //for (int i=0; i<4; i++) {
            //    Serial.print(packet.data[i], HEX);
            //    Serial.print(" ");
            //}
            //Serial.println();

            uint8_t cellNum = (packet.data[0] >> 4) & 0x0F;      // upper 4 bits
            uint8_t firstCellNo = packet.data[0] & 0x0F;         // lower 4 bits
            uint16_t rawCell1 = packet.data[1] + ((packet.data[2] & 0x0F) << 8);
            uint16_t rawCell2 = ((packet.data[2] >> 4) & 0x0F) + (packet.data[3] << 4);

            const float cell1 = rawCell1 / 500.0f;
            const float cell2 = rawCell2 / 500.0f;

            Serial.print("First cell: ");
            Serial.print(firstCellNo);
            Serial.print("\tCell Num: ");
            Serial.print(cellNum);
            Serial.print("\tCell1: ");
            Serial.print(cell1);
            Serial.print("\tCell2: ");
            Serial.println(cell2);

        }






    private:


        const FrskySensorPacket readPacket() {

            uint8_t rxdata[8];
            if (m_sport.available() == 8) {
                for (int i=0; i<8; i++) {
                    rxdata[i] = m_sport.read();
                }
            }

            // Flush just in case
            while (m_sport.available() > 0) {
                m_sport.read();
            }

            FrskySensorPacket packet;
            memcpy(&packet, rxdata, 8);

            return packet;

        }
        
        FrskySP m_sport;



    };



}



#endif