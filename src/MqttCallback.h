/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#ifndef MQTTCALLBACK_H
#define MQTTCALLBACK_H

#include "MqttClient.h"
#include <cstdint>

class MqttCallback : public MqttClient::Callback {
public:
        virtual ~MqttCallback () override = default;

        void onConnected (MqttClient &owner) override;
        void onDisconnected (MqttClient &) override {}
        void onData (MqttClient &owner, const char *topic, uint8_t const *data, size_t len) override;
        void onError (MqttClient &owner, MqttClient::Callback::Error error) override;
};

#endif // MQTTCALLBACK_H
