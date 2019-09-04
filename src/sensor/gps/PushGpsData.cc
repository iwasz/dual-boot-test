/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "PushGpsData.h"
#include "Debug.h"

bool PushGpsData::run (const EventType &)
{
#if 1
        if (!currentData->isValid ()) {
                return true;
        }
#endif

        if (gpsDataQueue->full ()) {
                // Kolejka jest pełna.
                debug->println ("Gps queue overflow");
                return true;
        }

        gpsDataQueue->push (*currentData);

#if 1
        uint8_t const &gpsQueueSize = uint8_t (gpsDataQueue->size ());
        debug->print ("Gps queue size : ");
        debug->println (gpsQueueSize);
#endif

        return true;
}
