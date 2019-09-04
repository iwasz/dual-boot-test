/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "MqttCallback.h"
#include "Debug.h"
#include "IumCborProtocolListener.h"
#include "cbor.h"

/*****************************************************************************/

void MqttCallback::onConnected (MqttClient &owner)
{
        debug->println ("Mqtt::onConnected.");
        /*
         * TODO Umieszczenie tego tutaj jest korzystne, bo mamy pewność, że po każdym połączeniu
         * aplikacja się zasubskrybuje. Ale powinna być możliwość odpalenia tego z main.
         */
        owner.subscribe ("config");
}

/*****************************************************************************/

void MqttCallback::onData (MqttClient &owner, const char *topic, uint8_t const *data, size_t len)
{
        debug->print ("Mqtt::onData, topic : ");
        debug->println (topic);
        cbor::input input (data, int(len));
        IumCborProtocolListener listener;
        cbor::decoder decoder (input, listener);
        decoder.run ();
}

/*****************************************************************************/

void MqttCallback::onError (MqttClient &owner, MqttClient::Callback::Error error)
{
        debug->print ("Mqtt::onError. Error =  ");
        debug->println ((int)error);
}
