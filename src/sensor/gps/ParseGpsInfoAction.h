/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef PARSEGPSINFOACTION_H
#define PARSEGPSINFOACTION_H

#include "Action.h"
#include "GpsData.h"

/**
 * Parsuje.
 */
class ParseGpsInfoAction : public Action<> {
public:
        enum Type { GGA, RMC, GSV };

        ParseGpsInfoAction (GpsData *c, Type t) : currentData (c), type (t) {}
        virtual ~ParseGpsInfoAction () {}
        bool run (EventType const &event);

private:
        GpsData *currentData;
        Type type;
};

#endif // PARSEGPSINFOACTION_H
