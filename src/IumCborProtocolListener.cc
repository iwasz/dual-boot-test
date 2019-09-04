/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "IumCborProtocolListener.h"
#include "Config.h"
#include "Debug.h"

#include "mc60Modem/Mc60Modem.h"

/*****************************************************************************/

void IumCborProtocolListener::on_map (int size)
{
        if (state == State::IDLE) {
                state = State::WAITING_FOR_COMMAND;
        }
        //        else if (state == State::WAITING_FOR_ARG_COLLECTION) {
        //                remainingArgs = size;
        //                state = (remainingArgs == 0) ? (State::IDLE) : (State::WAITING_FOR_ARG);
        //        }
        else {
                debug->print ("CBOR on_map, syntax error. Map size : ");
                debug->println (size);
        }
}

/*****************************************************************************/

void IumCborProtocolListener::on_array (int size)
{
        if (state == State::WAITING_FOR_ARG_COLLECTION) {
                remainingArgs = size;
                state = (remainingArgs == 0) ? (State::IDLE) : (State::WAITING_FOR_ARG);
        }
        else {
                debug->print ("CBOR on_array, syntax error. Map size : ");
                debug->println (size);
        }
}

/*****************************************************************************/

template <typename T> void IumCborProtocolListener::addWithCheck (T const &v)
{
        if (stack.full ()) {
                debug->print ("CBOR on_array, syntax error. Map size : ");
                reset ();
                return;
        }

        stack.push_back (Variant{ v });
}

/*****************************************************************************/

void IumCborProtocolListener::reset ()
{
        stack.clear ();
        state = State::IDLE;
        remainingArgs = 0;
}

/*****************************************************************************/

void IumCborProtocolListener::on_integer (int value)
{
        // debug->print ("CBOR integer : ");
        // debug->println (value);

        if (state == State::WAITING_FOR_ARG) {
                addWithCheck (value);

                if (--remainingArgs == 0) {
                        if (!onCommand (currentCommandName, stack)) {
                                debug->print ("Command argument mismatch. Command : ");
                                debug->println (currentCommandName.c_str ());
                        }

                        reset ();
                }
        }
        else {
                debug->print ("CBOR on_integer, syntax error. Map size : ");
                debug->println (value);
        }
}

/*****************************************************************************/

void IumCborProtocolListener::on_string (std::string &str)
{
        if (state == State::WAITING_FOR_COMMAND) { // Command name
                currentCommandName = str.c_str ();
                state = State::WAITING_FOR_ARG_COLLECTION;
        }
        else if (state == State::WAITING_FOR_ARG) {
                addWithCheck (string (str.c_str ()));

                if (--remainingArgs == 0) {
                        if (!onCommand (currentCommandName, stack)) {
                                debug->print ("Command argument mismatch. Command : ");
                                debug->println (currentCommandName.c_str ());
                        }

                        reset ();
                }
        }
        else {
                debug->print ("CBOR on_integer, syntax error. Map size : ");
                debug->println (str.c_str ());
        }

        //        debug->print ("CBOR string : ");
        //        debug->println (str.c_str ());
        //        lastElement = str.c_str ();
}

/*****************************************************************************/

void IumCborProtocolListener::on_error (const char *error)
{
        debug->print ("CBOR decoding error : ");
        debug->println (error);
}

/*****************************************************************************/

bool IumCborProtocolListener::onCommand (string const &commandName, Stack const &s)
{
        if (commandName == "blink") {
                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                Variant const &v = stack.at (0);
                config.blinkFrequencyMs = uint16_t (std::get<int> (v));
        }
        else if (commandName == "telemetryTimeout") {
                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                Variant const &v = stack.at (0);
                config.telemetryTimeoutMs = size_t (std::get<int> (v));
        }
        else if (commandName == "accelTimeout") {
                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                Variant const &v = stack.at (0);
                config.accelerometerTimeoutMs = size_t (std::get<int> (v));
        }
        else if (commandName == "flashTimeout") {
                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                Variant const &v = stack.at (0);
                config.flashTimeoutMs = size_t (std::get<int> (v));
        }
        else if (commandName == "power_18") {
                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                Variant const &v = stack.at (0);
                config.power18 = bool(std::get<int> (v));
        }
        else if (commandName == "power_33") {
                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                Variant const &v = stack.at (0);
                config.power33 = bool(std::get<int> (v));
        }
        else if (commandName == "gsm_power") {
                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                Variant const &v = stack.at (0);
                bool on = bool(std::get<int> (v));
                modem->power (on);
        }
        else if (commandName == "set_mode") {
                if (!mainStateMachine) {
                        debug->print ("No main machine");
                        return false;
                }

                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                mainStateMachine->setMode (MainStateMachine::Mode (std::get<int> (stack.at (0))));
                debug->println ("mode set OK");
        }
        else if (commandName == "get_mode") {
                if (!flash) {
                        debug->print ("No main machine");
                        return false;
                }

                debug->print ("current_mode : ");
                debug->println (int(mainStateMachine->getMode ()));
        }
        else if (commandName == "flash_erase4k") {
                if (!flash) {
                        debug->print ("No flash is set");
                        return false;
                }

                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                flash->erase4k (uint32_t (std::get<int> (stack.at (0))));
                debug->println ("flash_erase4k OK");
        }
        else if (commandName == "flash_write") {
                if (!flash) {
                        debug->print ("No flash is set");
                        return false;
                }

                if (!checkSize (s, 2) || !checkType<int> (s, 0) || !checkType<string> (s, 1)) {
                        return false;
                }

                uint32_t addr = uint32_t (std::get<int> (stack.at (0)));
                string const &s = std::get<string> (stack.at (1));
                flash->write (addr, reinterpret_cast<uint8_t const *> (s.c_str ()), s.size ());
                debug->println ("flash_write OK");
        }
        else if (commandName == "flash_read") {
                if (!flash) {
                        debug->print ("No flash is set");
                        return false;
                }

                if (!checkSize (s, 1) || !checkType<int> (s, 0)) {
                        return false;
                }

                uint32_t addr = uint32_t (std::get<int> (stack.at (0)));
                uint8_t buf[string::MAX_SIZE];
                flash->read (addr, buf, sizeof (buf));
                debug->print ("flash_read OK : ");
                debug->printArray (buf, sizeof (buf));
                debug->print ("\n");
        }

        return true;
}
