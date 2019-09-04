/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef PUSH_GPS_DATA_H
#define PUSH_GPS_DATA_H

#include "GpsData.h"
#include "Action.h"

/**
 * Parsuje.
 */
class PushGpsData : public Action<> {
public:
        PushGpsData (GpsData *c, GpsDataQueue *gpsDataQ/*, Rtc *r*/) : currentData (c), gpsDataQueue (gpsDataQ)/*, rtc (r)*/ {}
        virtual ~PushGpsData () {}
        bool run (EventType const &event);

private:
        GpsData *currentData;
        GpsDataQueue *gpsDataQueue;
//        Rtc *rtc;
};

#endif // PushGpsData_H
