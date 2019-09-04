/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef LIB_MY_MQTTCLIENT_H
#define LIB_MY_MQTTCLIENT_H

#include "Timer.h"
#include "ICommunicationInterface.h"
#include <cstdint>

class MqttClient : public ICommunicationInterface::Callback {
public:
        struct Callback {
                enum class Error : uint8_t { NO_ERROR };

                virtual ~Callback () = default;

                virtual void onConnected (MqttClient &owner) = 0;
                virtual void onDisconnected (MqttClient &owner) = 0;
                /// When broker publishes some data to a topic we subscribed for.
                virtual void onData (MqttClient &owner, const char *topic, uint8_t const *data, size_t len) = 0;
                virtual void onError (MqttClient &owner, Error error) = 0;
        };

        MqttClient (ICommunicationInterface &c, Callback *cal = nullptr) : callback (cal), connection (c) {}
        virtual ~MqttClient () override = default;

        void connect (const char *server, uint16_t port);
        void disconnect ();
        void subscribe (const char *topic);
        void publish (const char *topic, uint8_t *data, size_t dataLen);

        /*---------------------------------------------------------------------------*/

        /// This should go into main loop.
        void run ();

        /// Set a callback. Not obligatory.
        void setCallback (Callback *c) { callback = c; }

        /// Returns if client is connected to the broker in a TCP and MQTT sense.
        bool isMqttConnected () const { return mqttConnected; }

        /*---------------------------------------------------------------------------*/

        static constexpr size_t PING_DELAY_MS = 10 * 1000;

protected:
        // ICommunicationInterface::Callback
        virtual void onConnected (int connectionId) override;
        virtual void onDisconnected (int connectionId) override;
        virtual void onData (uint8_t const *data, size_t len) override;
        virtual void onError (ICommunicationInterface::Callback::Error error) override;

        Callback *callback;

private:
        static int transportRead (unsigned char *outBuf, int lenToRead);

private:
        ICommunicationInterface &connection;
        Timer pingTimer;
        int connectionId = -1;
        static uint8_t const *currentData;
        static int currentDataLen;

        // TODO State management - consider using state machine if there is too much of them.
        bool mqttConnected = false;
};

#endif // MQTTCLIENT_H
