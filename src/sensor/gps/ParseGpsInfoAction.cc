/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "ParseGpsInfoAction.h"
#include "Debug.h"
#include "minmea.h"

bool ParseGpsInfoAction::run (const EventType &event)
{
        if (type == RMC) {
                struct minmea_sentence_rmc frame;

                if (!minmea_parse_rmc (&frame, event.c_str ())) {
                        return true;
                }

                currentData->latitude = minmea_tocoord (&frame.latitude);
                currentData->longitude = minmea_tocoord (&frame.longitude);
                currentData->speed = minmea_rescale (&frame.speed, 100);
                // currentData->microDate.yearPast2015 = frame.date.year - 15;
                // currentData->microDate.month = frame.date.month;
                // currentData->microDate.day = frame.date.day;
                // currentData->microDate.hour = frame.time.hours;
                // currentData->microDate.minute = frame.time.minutes;
                // currentData->microDate.second = frame.time.seconds;
        }
        else if (type == GGA) {
                struct minmea_sentence_gga frame;

                if (!minmea_parse_gga (&frame, event.c_str ())) {
                        return true;
                }

                currentData->altitude = minmea_tocoord (&frame.altitude);
        }
        else if (type == GSV) {
                struct minmea_sentence_gsv frame;

                if (!minmea_parse_gsv (&frame, event.c_str ())) {
                        return true;
                }

                // currentData->altitude = minmea_tocoord (&frame.altitude);
                debug->print ("Total satellites : ");
                debug->println (frame.total_sats);
        }
        else {
                Error_Handler ();
        }

        return true;
}
