/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef BG96_H
#define BG96_H

#include "StateMachine.h"
#include "Usart.h"
#include "character/LineSink.h"
#include "collection/CircularBuffer.h"
#include "sensor/gps/GpsData.h"

/**
 * @brief The IGnss struct
 */
struct IGnss {
        virtual ~IGnss () = default;
};

/**
 * @brief The Bg96Gnss class
 */
class Bg96Gnss : public IGnss {
public:
        static constexpr uint32_t MACHINE_LOG_ID = 1;

        Bg96Gnss (Usart &u);
        void run () { machine.run (); }

        GpsDataQueue const &getGnssDataQueue () const { return gnssDataQueue; }
        GpsDataQueue &getGnssDataQueue () { return gnssDataQueue; }

protected:
        StateMachine<>::EventQueue &getEventQueue () { return machine.getEventQueue (); }

private:
        // Kolejka ze sparsowanymi danymi GPS . TODO zastanowić się nad API.
        GpsDataQueue gnssDataQueue;
        StateMachine<> machine;
        LineSink<StateMachine<>::EventQueue, string> gnssResponseSink; // NMEA responses from the GNSS.
};

#endif // BG96_H
