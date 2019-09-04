/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Bg96Gnss.h"
#include "BeginsWithCondition.h"
#include "BoolCondition.h"
#include "DelayAction.h"
#include "GpsData.h"
#include "StringCondition.h"
#include "UsartAction.h"
#include "sensor/gps/ParseGpsInfoAction.h"
#include "sensor/gps/PushGpsData.h"

/*****************************************************************************/

enum MachineState : size_t { GPS_USART_RESTART, GPS_WAIT_FOR_DATA, GPS_GET_GGA, GPS_GET_RMC, GPS_GET_GSV, GPS_PUSH_BACK, GPS_USART_SHUT_DOWN };

/*****************************************************************************/

Bg96Gnss::Bg96Gnss (Usart &u) : machine (MACHINE_LOG_ID), gnssResponseSink (getEventQueue ())
{
        u.setSink (&gnssResponseSink);

        /*
         * L80 zwraca:
         * $GPGGA
         * $GPGLL
         * $GPGSA
         * $GPGSV
         * $GPRMC
         * $GPTXT
         * $GPVTG
         * UWAGA! Nie zwraca nic co się zaczyna od GN (jak glonas). Ponieważ to jest single constellation GPS.
         */
        static GpsData currentGpsData;
        static UsartAction<string> initGpsUsart (u, UsartAction<string>::INTERRUPT_ON);

        /* clang-format off */

        // machine.state (GPS_USART_RESTART)->entry (and_action (and_action (&initGpsUsart, delayMs (100)), gps ("\r\n")))
        machine.state (GPS_USART_RESTART, StateFlags::INITIAL)->entry (&initGpsUsart)
                ->transition (GPS_WAIT_FOR_DATA)->when (&alwaysTrue)->then (delayMs (2000));

        /*
         * Uwaga, to się musi wykonywać dość często, bo inaczej przepełni się kolejka wejściowa.
         * Ta maszyna ma ustawione, żeby przy przejściu zdejmować tylko najstarsze wejście (aby
         * nie utracić żadnego). Jednocześnie GPS powinien wypluwać tylko 2 wyjścia na sekundę
         * (reszta jest odfiltrowywana w ISR od USART4). Tak więc przy opóźnieniu mniejszym niż
         * 500ms powinno działać ok. Tu mam ustawione na 300ms
         */
        machine.state (GPS_WAIT_FOR_DATA)/*->entry (&wdgRefreshAction)*/
                ->transition (GPS_GET_GGA)->when (beginsWith ("$GPGGA", StripInput::STRIP, InputRetention::RETAIN_INPUT))
                ->transition (GPS_GET_GGA)->when (beginsWith ("$GPGGA", StripInput::STRIP, InputRetention::RETAIN_INPUT))
                ->transition (GPS_GET_RMC)->when (beginsWith ("$GNRMC", StripInput::STRIP, InputRetention::RETAIN_INPUT))
                ->transition (GPS_GET_RMC)->when (beginsWith ("$GPRMC", StripInput::STRIP, InputRetention::RETAIN_INPUT))
                ->transition (GPS_GET_GSV)->when (beginsWith ("$GPGSV", StripInput::STRIP, InputRetention::RETAIN_INPUT));

        static ParseGpsInfoAction parseGGA (&currentGpsData, ParseGpsInfoAction::GGA);
        machine.state (GPS_GET_GGA)->exit (&parseGGA)
                ->transition (GPS_WAIT_FOR_DATA)->when (&alwaysTrue);

        static ParseGpsInfoAction parseRMC (&currentGpsData, ParseGpsInfoAction::RMC);
        machine.state (GPS_GET_RMC)->exit (&parseRMC)
                ->transition (GPS_PUSH_BACK)->when (&alwaysTrue);

        static ParseGpsInfoAction parseGSV (&currentGpsData, ParseGpsInfoAction::GSV);
        machine.state (GPS_GET_GSV)->exit (&parseGSV)
                ->transition (GPS_WAIT_FOR_DATA)->when (&alwaysTrue);

        static PushGpsData pushBackGps (&currentGpsData, &gnssDataQueue);
        machine.state (GPS_PUSH_BACK)->exit (&pushBackGps)->transition (GPS_WAIT_FOR_DATA)->when (&alwaysTrue);

        // TODO GPS nie reaguje na wysyłane komendy!!!
        static UsartAction<string> deinitGpsUsart (u, UsartAction<string>::INTERRUPT_OFF);
        // machine.state (GPS_USART_SHUT_DOWN)->entry (and_action (and_action (gps ("$PMTK161,1*28\r\n"), delayMs (100)), &deinitGpsUsart));
        machine.state (GPS_USART_SHUT_DOWN)->entry (&deinitGpsUsart);

        /* clang-format on */
}
