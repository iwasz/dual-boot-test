/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include "Timer.h"
#include "Usart.h"
#include "character/ICharacterSink.h"
#include <cstdint>
#include <cstring>
#include <etl/deque.h>
#include <etl/vector.h>

/**
 * TODO Decide whether to turn off the interrupts inside onData to prevent priority inversion.
 * TODO Allow messages bigger than 127.
 * TODO maybe templatize ELEMENT_MAX_SIZE and QUEUE_MAX_SIZE
 */
template <typename El = uint8_t, typename IsrCtrl = CortexMInterruptControl> class PacketSink : public ICharacterSink {
public:
        enum class State { IDLE, RECEIVING };
        static constexpr size_t TIMEOUT_MS = 10;
        static constexpr size_t ELEMENT_MAX_SIZE = 128;
        using ElStr = etl::vector<El, ELEMENT_MAX_SIZE>;
        static constexpr size_t QUEUE_MAX_SIZE = 3;
        using ElStrQue = etl::deque<ElStr, QUEUE_MAX_SIZE>;

        PacketSink () = default;
        PacketSink (PacketSink const &) = delete;
        PacketSink (PacketSink &&) = delete;
        PacketSink &operator= (PacketSink const &) = delete;
        PacketSink &operator= (PacketSink &&) = delete;
        virtual ~PacketSink () override = default;

        virtual void onData (uint8_t c) override;
        virtual void onError (uint32_t flags) override;

        /// Check for timeouts
        void run () {}

        bool hasData () const
        {
                InterruptLock<IsrCtrl> lock;
                bool em = queue.empty ();
                return !em;
        }

        ElStrQue const &getQueue () const { return queue; }

        void pop ()
        {
                InterruptLock<IsrCtrl> lock;
                queue.pop_front ();
        }

private:
        void reset ();

private:
        Timer timer;
        State state = State::IDLE;
        uint8_t bytesToGo = 0;
        bool moreFramesToCome = false;

        ElStr current;
        ElStrQue queue;
};

/*---------------------------------------------------------------------------*/

template <typename El, typename IsrCtrl> void PacketSink<El, IsrCtrl>::onData (uint8_t c)
{
        if (state == State::IDLE) {
                current.clear ();
                state = State::RECEIVING;
                bytesToGo = c & 0x7f;
                moreFramesToCome = c & 0x80;
                timer.start (TIMEOUT_MS);

                if (bytesToGo == 0) {
                        reset ();
                        return;
                }
        }
        else if (state == State::RECEIVING) {

                if (current.full ()) {
                        onError (1);
                        reset ();
                        return;
                }

                if (timer.isExpired ()) {
                        onError (3);
                        reset ();
                        return;
                }

                current.push_back (c);

                if (current.size () == bytesToGo) {
                        state = State::IDLE;

                        if (queue.full ()) {
                                onError (2);
                                reset ();
                                return;
                        }

                        // Probably ISR OFF
                        // Warning : copying is performed here.
                        queue.push_back (current);
                        // Probably ISR ON
                }
        }
}

/*---------------------------------------------------------------------------*/

template <typename El, typename IsrCtrl> void PacketSink<El, IsrCtrl>::onError (uint32_t c) {}

/*---------------------------------------------------------------------------*/

template <typename El, typename IsrCtrl> void PacketSink<El, IsrCtrl>::reset ()
{
        state = State::IDLE;
        bytesToGo = 0;
        moreFramesToCome = false;
}
