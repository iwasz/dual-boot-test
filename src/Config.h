/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#pragma once
#include <cstdint>
#include <cstdlib>

/// System configuration.
struct Config {
        size_t telemetryTimeoutMs = 5 * 1000;
        uint16_t blinkFrequencyMs = 0;

        // Czy uruchomić testowy odczyt z akcelerometru.
        size_t accelerometerTimeoutMs = 0;

        // Czy uruchomić testowy zapis / odczyt z FLASH.
        size_t flashTimeoutMs = 0;

        bool power18 = true;
        bool power33 = true;
};

/// Global config.
extern Config config;
