/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once

/*****************************************************************************/

class MainStateMachine {
public:
        enum class Mode { RUCH, POSTOJ, MAGAZYN, SERWIS };

        Mode getMode () const { return mode; }
        void setMode (Mode m) { mode = m; }

private:
        Mode mode = Mode::SERWIS;
};
