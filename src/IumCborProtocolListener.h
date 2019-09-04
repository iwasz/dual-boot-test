/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/
#ifndef IUMCBORPROTOCOLLISTENER_H
#define IUMCBORPROTOCOLLISTENER_H

#include "cbor.h"
// TODO string. String powinien być w innym pliku.
#include "MainStateMachine.h"
#include "SerialFlash.h"
#include "StateMachineTypes.h"
#include <etl/vector.h>
#include <string>
#include <variant>

class Mc60Modem;

/**
 * SAX like callback class for parsing CBOR.
 * Commands for IUM looks like this:
 * // {"commandName" : {"arg1Name":"valueStr", "arg2Name":valueInt}}
 * {"commandName" : ["valueStr", valueInt]}
 *
 * TODO They can be chained inside an array:
 * [{}, {}]
 *
 * Restrictions:
 * - Strings has to be no longer than 16B
 * - If a command requires "bytes" argument, it has to be provided as a last one
 * (to prevent copying it onto the stack).
 */
class IumCborProtocolListener : public cbor::listener {
public:
        enum class State { IDLE, WAITING_FOR_COMMAND, WAITING_FOR_ARG_COLLECTION, WAITING_FOR_ARG };
        using string = etl::string<16>;
        using Variant = std::variant<int, string, float>;
        using Stack = etl::vector<Variant, 4>;

        IumCborProtocolListener (SerialFlash *flash = nullptr, MainStateMachine *m = nullptr, Mc60Modem *md = nullptr)
            : flash (flash), mainStateMachine (m), modem (md)
        {
        }
        virtual ~IumCborProtocolListener () = default;

        void on_integer (int value);
        void on_bytes (unsigned char *data, int size) {}
        // TODO get rid of this std::string!
        void on_string (std::string &str);
        void on_array (int size);
        void on_map (int size);
        void on_tag (unsigned int tag) {}
        void on_special (unsigned int code) {}
        void on_bool (bool) {}
        void on_null () {}
        void on_undefined () {}
        void on_error (const char *error);
        void on_extra_integer (unsigned long long value, int sign) {}
        void on_extra_tag (unsigned long long tag) {}
        void on_extra_special (unsigned long long tag) {}

        /* virtual */ bool onCommand (string const &, Stack const &s) /* = 0*/;

private:
        template <typename T> void addWithCheck (T const &v);
        void reset ();
        bool checkSize (Stack const &s, size_t size) { return s.size () == size; }
        template <typename T> bool checkType (Stack const &s, size_t idx) { return std::holds_alternative<T> (s.at (idx)); }

private:
        string currentCommandName;
        State state = State::IDLE;
        int remainingArgs = 0;
        Stack stack;
        SerialFlash *flash;                 // TODO Zamienić na referencję, kiedy nie będzie opcjonalne.
        MainStateMachine *mainStateMachine; // TODO Zamienić na referencję, kiedy nie będzie opcjonalne.
        // TODO zbyt duże zalezności, nie powonno tu być. Użyć kolejki komend, czy innych messages.
        Mc60Modem *modem;
};

#endif // IUMCBORPROTOCOLLISTENER_H
