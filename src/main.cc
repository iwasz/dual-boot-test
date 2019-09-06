/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Blinker.h"
#include "Debug.h"
//#include "Config.h"
#include "ErrorHandler.h"
#include "Gpio.h"
#include "Usart.h"
//#include "character/FixedLineSink.h"
#include <cerrno>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <esp8266/Esp8266.h>
//#include "BinaryEvent.h"
//#include <fmt/core.h>

/*****************************************************************************/

static void SystemClock_Config (void);

/*****************************************************************************/

// class MyPacketSink : public PacketSink<> {
// public:
//        using PacketSink::PacketSink;
//        virtual ~MyPacketSink () = default;
//        virtual void onError (uint32_t flags) override
//        {
//                debug->print ("PacketSink error : ");
//                debug->println (flags);
//        }
//};

int main ()
{
        HAL_Init ();
        SystemClock_Config ();
        HAL_Delay (100);

        Gpio led (GPIOA, GPIO_PIN_0);
        Blinker<4> blink (led, 1000, 50, 100, 50);

        Gpio debugUartGpios (GPIOA, GPIO_PIN_2 | GPIO_PIN_3, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART2);
        Usart debugUart (USART2, 115200);

        Debug debug (&debugUart);
        Debug::singleton () = &debug;
        ::debug = Debug::singleton ();
        ::debug->print ("     _             _ _                 _   \r\n"
                        "  __| |_   _  __ _| | |__   ___   ___ | |_ \r\n"
                        " / _` | | | |/ _` | | '_ \\ / _ \\ / _ \\| __|\r\n"
                        "| (_| | |_| | (_| | | |_) | (_) | (_) | |_ \r\n"
                        " \\__,_|\\__,_|\\__,_|_|_.__/ \\___/ \\___/ \\__|\r\n");

        //        fmt::format("The answer is {}.", 42);
        //        fmt::print ("asdasd");

        Gpio espUartGpios (GPIOA, GPIO_PIN_9 | GPIO_PIN_10, GPIO_MODE_AF_OD, GPIO_PULLUP, GPIO_SPEED_FREQ_HIGH, GPIO_AF7_USART1);
        Usart espUart (USART1, 115200);
        HAL_NVIC_SetPriority (USART1_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ (USART1_IRQn);
        Esp8266 esp8266 (espUart);
        espUart.startReceive ();

        Timer t;
        t.start (10000);

        Timer rxThroughput{ 1000 };
        size_t rxBytes = 0;

        while (true) {
                // blink.setTimeSlot (0, config.blinkFrequencyMs);

                //                if (config.blinkFrequencyMs > 0) {
                blink.run ();
                esp8266.run ();

                if (t.isExpired ()) {
                        static int i = 0;

                        if (i == 0 && !esp8266.isTcpConnected ()) {
                                esp8266.connect (nullptr, 0);
                        }

                        if (i == 1 && esp8266.isTcpConnected () && !esp8266.isSending ()) {
                                uint8_t buf[] = { 'a', 'l', 'a', ' ', 'm', 'a', 'k', 'o', 't', 'a', '\r', '\n' };
                                esp8266.send (buf);
                        }

                        //                        if (i == 2 && esp8266.isTcpConnected () && !esp8266.isSending ()) {
                        //                                esp8266.disconnect ();
                        //                        }

                        t.start (10000);
                        ++i;
                        i %= 3;
                }

                if (!esp8266.getReceiveBuffer ().empty ()) {
                        size_t size;

                        {
                                InterruptLock lock;
                                size = esp8266.getReceiveBuffer ().size ();
                                esp8266.getReceiveBuffer ().clear ();
                        }

                        rxBytes += size;
                }

                if (rxThroughput.isExpired ()) {
                        debug.print ("Rx : ");
                        debug.print (rxBytes);
                        debug.println ("B, ");
                        debug.print ("speed : ");
                        debug.print ((rxBytes * 8));
                        debug.println ("bps");
                        rxThroughput.start (1000);
                        rxBytes = 0;
                }

                //                }

                //                if (debugSink.hasData ()) {
                //                        auto const &data = debugSink.getQueue ().front ();
                //                        cbor::input input (data.data (), data.size ());
                //                        IumCborProtocolListener listener (&serialFlash, &mainStateMachine, &modem);
                //                        // IumCborProtocolListener listener (nullptr, &mainStateMachine);
                //                        cbor::decoder decoder (input, listener);
                //                        decoder.run ();
                //                        // debug.printArray (data.data (), data.size ());
                //                        debugSink.pop ();
                //                }
        }
}

/*****************************************************************************/

void SystemClock_Config (void)
{

        RCC_OscInitTypeDef RCC_OscInitStruct;
        RCC_ClkInitTypeDef RCC_ClkInitStruct;
        RCC_PeriphCLKInitTypeDef PeriphClkInit;

        /**Initializes the CPU, AHB and APB busses clocks
         */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
        RCC_OscInitStruct.HSIState = RCC_HSI_ON;
        RCC_OscInitStruct.HSICalibrationValue = 16;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
        RCC_OscInitStruct.PLL.PLLM = 1;
        RCC_OscInitStruct.PLL.PLLN = 10;
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
        RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
        RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

        if (HAL_RCC_OscConfig (&RCC_OscInitStruct) != HAL_OK) {
                abort ();
        }

        /**Initializes the CPU, AHB and APB busses clocks
         */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

        if (HAL_RCC_ClockConfig (&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
                abort ();
        }

        PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2;
        PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig (&PeriphClkInit) != HAL_OK) {
                abort ();
        }

        /**Configure the main internal regulator output voltage
         */
        if (HAL_PWREx_ControlVoltageScaling (PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK) {
                abort ();
        }
}

/*****************************************************************************/

namespace __gnu_cxx {
void __verbose_terminate_handler ()
{
        while (1)
                ;
}
} // namespace __gnu_cxx

// Potrzebne do cn-cbor, nieprzenosne, zadziała tylko z GCC!
extern "C" uint32_t htonl (uint32_t a) { return __builtin_bswap32 (a); }
extern "C" uint16_t htons (uint16_t a) { return __builtin_bswap16 (a); }
