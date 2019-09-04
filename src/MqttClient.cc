/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "MqttClient.h"
#include "Debug.h"
#include "ErrorHandler.h"
#include "MQTTPacket.h"
#include <algorithm>

/*****************************************************************************/

uint8_t const *MqttClient::currentData = { 0 };
int MqttClient::currentDataLen = 0;

/*****************************************************************************/

void MqttClient::connect (const char *server, uint16_t port) { connection.connect (server, port); }

/*****************************************************************************/

void MqttClient::disconnect () {}

/*****************************************************************************/

void MqttClient::subscribe (const char *topic)
{
        uint8_t encodedMqtt[2048];
        int encodedMqttLen = sizeof (encodedMqtt);
        MQTTString topicString = MQTTString_initializer;
        topicString.cstring = const_cast<char *> (topic);
        int requestedQoSs[] = { 0 };
        static constexpr int TOPICS_NUMBER = 1;
        unsigned short messageId = 1;
        unsigned char dupMqttFlag = 0;
        int len = MQTTSerialize_subscribe (encodedMqtt, encodedMqttLen, dupMqttFlag, messageId, TOPICS_NUMBER, &topicString, requestedQoSs);

        //                        topicString = MQTTString_initializer;
        //                        topicString.cstring = "iwasz";
        //                        len += MQTTSerialize_publish ((encodedMqtt + len), encodedMqttLen - len, 0, 0, 0, 0,
        //                        topicString, encodedCbor,
        //                                                      encodedCborLen);

        //                        len += MQTTSerialize_disconnect ((encodedMqtt + len), encodedMqttLen - len);

        connection.send (0, encodedMqtt, size_t (len));
}

/*****************************************************************************/

void MqttClient::publish (const char *topic, uint8_t *data, size_t dataLen)
{
        uint8_t encodedMqtt[2048];
        int encodedMqttLen = sizeof (encodedMqtt);
        MQTTString topicString = MQTTString_initializer;
        topicString.cstring = const_cast<char *> (topic);
        int len = MQTTSerialize_publish (encodedMqtt, encodedMqttLen, 0, 0, 0, 0, topicString, data, dataLen);

        if (len < 0) {
                Error_Handler ();
        }

        int sent = 0;
        if ((sent = connection.send (0, encodedMqtt, size_t (len))) != len) {
                Error_Handler ();
        }
}

/*****************************************************************************/

void MqttClient::run ()
{
        connection.run ();

        // Sending a ping
        if (mqttConnected && pingTimer.isExpired ()) {
                uint8_t encodedMqtt[64];
                int encodedMqttLen = sizeof (encodedMqtt);
                int len = MQTTSerialize_pingreq (encodedMqtt, encodedMqttLen);
                connection.send (0, encodedMqtt, size_t (len));
                pingTimer.start (PING_DELAY_MS);
        }
}

/*****************************************************************************/

void MqttClient::onConnected (int connectionId)
{
#if 1
        debug->println ("Mc60::onConnected.");
#endif

        this->connectionId = connectionId;
        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.clientID.cstring = "ium-lite";
        data.keepAliveInterval = 0;
        data.cleansession = 1;
        // data.username.cstring = "testuser";
        // data.password.cstring = "testpassword";
        data.username.cstring = nullptr;
        data.password.cstring = nullptr;
        data.MQTTVersion = 4;

        uint8_t encodedMqtt[2048];
        int encodedMqttLen = sizeof (encodedMqtt);
        int len = MQTTSerialize_connect (encodedMqtt, encodedMqttLen, &data);
        connection.send (connectionId, encodedMqtt, size_t (len));
}

/*****************************************************************************/

void MqttClient::onDisconnected (int connectionId) {}

/*****************************************************************************/

void MqttClient::onData (uint8_t const *data, size_t len)
{
        currentData = data;
        currentDataLen = len;

#if 1
        debug->print ("Mc60::onData, len : ");
        debug->print (len);
        debug->print (", [");
        debug->printArray (data, len);
        debug->println ("]");
#endif
        uint8_t encodedMqtt[2048];
        int encodedMqttLen = sizeof (encodedMqtt);

        while (currentDataLen > 0) {
                int packetType = MQTTPacket_read (encodedMqtt, encodedMqttLen, transportRead);

                switch (packetType) {
                case CONNACK: {
                        unsigned char sessionPresent, connack_rc;

                        // Is it necessary?
                        if (MQTTDeserialize_connack (&sessionPresent, &connack_rc, encodedMqtt, encodedMqttLen) != 1 || connack_rc != 0) {
                                debug->println ("Error decoding MQTT conack.");
                                return;
                        }

                        mqttConnected = true;
                        pingTimer.start (PING_DELAY_MS);

                        if (callback) {
                                callback->onConnected (*this);
                        }
                } break;

                case PINGRESP:
                        debug->println ("PINGRESP.");
                        break;

                case PUBLISH: {
                        if (!callback) {
                                break;
                        }

                        unsigned char dup = 0;
                        int qos = 0;
                        unsigned char retained = 0;
                        unsigned short packetid = 0;
                        MQTTString topicName;
                        unsigned char *payload = nullptr;
                        int payloadlen = 0;

                        // Is it necessary?
                        if (MQTTDeserialize_publish (&dup, &qos, &retained, &packetid, &topicName, &payload, &payloadlen, encodedMqtt,
                                                     encodedMqttLen)
                            != 1) {
                                debug->println ("Error decoding MQTT conack.");
                                return;
                        }

                        callback->onData (*this, topicName.cstring, payload, payloadlen);
                } break;

                case PUBACK:
                case PUBREC:
                case PUBREL:
                case PUBCOMP:
                case SUBACK:
                case UNSUBACK:
                        debug->println ("MQTT packet received, not implemented.");
                        break;

                case PINGREQ:
                case SUBSCRIBE:
                case CONNECT:
                case UNSUBSCRIBE:
                case DISCONNECT:
                        debug->println ("Unsupported MQTT packet");
                        break;

                default:
                        debug->println ("Unknown MQTT packet");
                        break;
                }
        }
}

/*****************************************************************************/

int MqttClient::transportRead (unsigned char *outBuf, int lenToRead)
{
        lenToRead = std::min (lenToRead, currentDataLen);
        std::copy (currentData, currentData + lenToRead, outBuf);
        currentData += lenToRead;
        currentDataLen -= lenToRead;
        return lenToRead;
}

/*****************************************************************************/

void MqttClient::onError (Error error)
{
        debug->print ("Mc60::onError. Error =  ");
        debug->println ((int)error);
}
