/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef DATA_STRUCT_GPSDATA_H
#define DATA_STRUCT_GPSDATA_H

#include <cstdint>
#include <etl/queue.h>

/**
 * Max długość wiersza (jednej linijki) odpowiedzi od GPS. Wsyztsko ponad ot
 * zostanie obcięte.
 */
#define MAX_CGPSINF_BUF_LEN 200

/**
 * TODO dodać pole z czasem
 */
#ifndef UNIT_TEST
struct GpsData {

        //        GpsData () : microDate () {}
        bool isValid () const { return latitude > 0 || longitude > 0 || speed > 0; }

        float longitude = 0;
        float latitude = 0;
        float altitude = 0;
        // float course = 0;
        // uint16_t timeToFirstFix = 0;
        // uint8_t sateliteNumber = 0;
        uint8_t speed = 0;
        // uint8_t mode = 0;
};
#else
// Unit testy zależą od rozmiaru tej struktury.
struct GpsData {

        bool isValid () const { return latitude > 0 || longitude > 0 || speed > 0; }

        float longitude = 0;
        float latitude = 0;
        float altitude = 0;
        MicroDate microDate;
        uint8_t speed = 0;
};
#endif

// typedef enum {
//        CGPS_PARSER_OK,
//        CGPS_NO_COMA_FOUND,
//        CGPS_LONGITUDE_PARISNG_ERROR,
//        CGPS_LATITUDE_PARISNG_ERROR,
//        CGPS_COORDINATE_PARISNG_ERROR,
//        CGPS_EINVAL,
//        CGPS_ERANGE,
//        CGPS_NO_CONVERSION
//} GpsParserStatus;

typedef etl::queue<GpsData, 16> GpsDataQueue;

/**
 * Parsuje linijkę i uzupełnia strukturę GpsData. Input powinien być bez \r i \n.
 */
// extern GpsParserStatus parseGpsInfo (const char *buf, GpsData *gpsData);

#endif // GPSDATA_H
