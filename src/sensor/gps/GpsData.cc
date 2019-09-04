/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include "GpsData.h"

//#define DEG_MAX_CHARS 4

/****************************************************************************/

#if 0
static float coordinateFromString (const char *buf, bool *status)
{
        float coord = 0.0;
        const char *begin = buf;
        const char *dot = strchr (begin, '.');
        bool negative = false;
        // Liczba znaków potrzebnych do zapisania pełnych stopni.
        int8_t degLen = dot - begin - 2;

        if (degLen > DEG_MAX_CHARS) {
                *status = false;
                return 0.0;
        }

        if (begin[0] == '-') {
                negative = true;
        }

        if (degLen > 0) {
                memcpy (tmpBuf, begin, degLen);
                tmpBuf[degLen] = '\0';
                coord = strtof (tmpBuf, NULL);
                begin += degLen;
        }

        if (/*coord >= 0*/ !negative) {
                coord += strtof (begin, NULL) / 60.0;
        }
        else {
                coord -= strtof (begin, NULL) / 60.0;
        }

        return coord;
}
#endif
/****************************************************************************/
#if 0
void nextToken (char **begin, char **end)
{
        *end = strchr (*begin, ',');

        if (*end) {
                **end = '\0';
        }
}

/****************************************************************************/

void skipToken (char **begin, char **end)
{
        *end = strchr (*begin, ',');

        if (*end) {
                *begin = *end + 1;
        }
}

/****************************************************************************/

// Modyfikuje begin
uint32_t integerFromToken (char **begin, char **end, GpsParserStatus *status)
{
        nextToken (begin, end);
        errno = 0;

        uint32_t ret = strtoul (*begin, NULL, 10);

        if (errno == EINVAL) {
                *status = CGPS_EINVAL;
                return 0;
        }
        else if (errno == ERANGE) {
                *status = CGPS_ERANGE;
                return 0;
        }

        if (*end) {
                *begin = *end + 1;
        }
        return ret;
}

/*--------------------------------------------------------------------------*/

// Modyfikuje begin
float floatFromToken (char **begin, char **end, GpsParserStatus *status)
{
        nextToken (begin, end);

        errno = 0;
        char *endptr = NULL;

        float ret = strtof (*begin, &endptr);

        if (errno == ERANGE) {
                *status = CGPS_ERANGE;
                return 0;
        }

        if (ret == 0.0 && endptr == *begin) {
                *status = CGPS_NO_CONVERSION;
        }

        if (*end) {
                *begin = *end + 1;
        }
        return ret;
}

/*--------------------------------------------------------------------------*/

// Modyfikuje begin
float coordinateFromToken (char **begin, char **end, GpsParserStatus *status)
{
        nextToken (begin, end);

        bool coordinateParsingStatus = true;

        float ret = coordinateFromString (*begin, &coordinateParsingStatus);

        if (!coordinateParsingStatus) {
                return CGPS_COORDINATE_PARISNG_ERROR;
        }

        if (*end) {
                *begin = *end + 1;
        }
        return ret;
}
#endif
/****************************************************************************/

#if 0
GpsParserStatus parseGpsInfo (const char *buf, GpsData *gpsData)
{
        GpsParserStatus status = CGPS_PARSER_OK;
        char bufCopy[MAX_CGPSINF_BUF_LEN + 1];
        char *end = NULL;
        char *begin = bufCopy;
        strncpy (bufCopy, buf, MAX_CGPSINF_BUF_LEN);
        /*
         * From man-page : "Warning: If there is no null byte among the first n bytes of src,
         * the string placed in dest will not be null-terminated."
         */
        begin[MAX_CGPSINF_BUF_LEN] = '\0';

        // gpsData->mode = integerFromToken (&begin, &end, &status);
        skipToken (&begin, &end);
        skipToken (&begin, &end);

        /*--------------------------------------------------------------------------*/

        end = strchr (begin, ',');
        if (end) {
                *end = '\0';
                gpsStringToMicroDate (begin, &gpsData->microDate);
                begin = end + 1;
        }

        /*--------------------------------------------------------------------------*/

        gpsData->latitude = floatFromToken (&begin, &end, &status);
        gpsData->longitude = floatFromToken (&begin, &end, &status);
        gpsData->altitude = floatFromToken (&begin, &end, &status);
        gpsData->speed = floatFromToken (&begin, &end, &status);
        return status;
}
#endif
