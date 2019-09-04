/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "Lis3dhSpiBsp.h"

/*****************************************************************************/

Lis3dhSpiBsp *Lis3dhSpiBsp::instance;

/*****************************************************************************/

void Lis3dhSpiBsp::itConfig ()
{
        //        GPIO_InitTypeDef gpioInitStructure;

        //        /* Enable INT2 GPIO clock and configure GPIO PINs to detect Interrupts */
        //        ACCELERO_INT_GPIO_CLK_ENABLE ();

        //        /* Configure GPIO PINs to detect Interrupts */
        //        gpioInitStructure.Pin = ACCELERO_INT1_PIN | ACCELERO_INT2_PIN;
        //        gpioInitStructure.Mode = GPIO_MODE_IT_RISING;
        //        gpioInitStructure.Speed = GPIO_SPEED_HIGH;
        //        gpioInitStructure.Pull = GPIO_PULLDOWN;
        //        HAL_GPIO_Init (ACCELERO_INT_GPIO_PORT, &gpioInitStructure);

        //        /* Enable and set Accelerometer INT2 to the lowest priority */
        //        HAL_NVIC_SetPriority ((IRQn_Type)ACCELERO_INT2_EXTI_IRQn, 0x01, 0);
        //        HAL_NVIC_EnableIRQ ((IRQn_Type)ACCELERO_INT2_EXTI_IRQn);

        //        HAL_NVIC_SetPriority ((IRQn_Type)ACCELERO_INT1_EXTI_IRQn, 0x01, 0);
        //        HAL_NVIC_EnableIRQ ((IRQn_Type)ACCELERO_INT1_EXTI_IRQn);
}

/*****************************************************************************/

/**
 * @brief  Sends a Byte through the SPI interface and return the Byte received.
 *         from the SPI bus.
 * @param  Byte: Byte send.
 * @retval The received byte value
 */
uint8_t Lis3dhSpiBsp::spiWriteRead (uint8_t byte)
{
        spi.setNss (false);
        uint8_t ret = spi.transmit8 (byte);
        spi.setNss (true);
        return ret;
}

/*****************************************************************************/

void Lis3dhSpiBsp::write (uint8_t *buffer, uint8_t addr, uint16_t len)
{
        /* Configure the MS bit:
           - When 0, the address will remain unchanged in multiple read/write commands.
           - When 1, the address will be auto incremented in multiple read/write commands.
        */
        if (len > 0x01) {
                addr |= (uint8_t)MULTIPLEBYTE_CMD;
        }

        spi.setNss (false);
        spi.transmit8 (addr);
        spi.transmit8 (buffer, len);
        spi.setNss (true);
}

/*****************************************************************************/

void Lis3dhSpiBsp::read (uint8_t *buffer, uint8_t addr, uint16_t len)
{
        if (len > 0x01) {
                addr |= (uint8_t) (READWRITE_CMD | MULTIPLEBYTE_CMD);
        }
        else {
                addr |= (uint8_t)READWRITE_CMD;
        }

        spi.setNss (false);
        spi.transmit8 (addr);
        spi.receive8 (buffer, len);
        spi.setNss (true);
}

/*****************************************************************************/

extern "C" void ACCELERO_INT1_HANDLER ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (ACCELERO_INT1_PIN);
        Lis3dhSpiBsp *bsp = Lis3dhSpiBsp::singleton ();
        if (bsp->lis3dshBspListener) {
                bsp->lis3dshBspListener->onInterrupt1 ();
        }
}

/*****************************************************************************/

extern "C" void ACCELERO_INT2_HANDLER ()
{
        __HAL_GPIO_EXTI_CLEAR_IT (ACCELERO_INT2_PIN);
        Lis3dhSpiBsp *bsp = Lis3dhSpiBsp::singleton ();
        if (bsp->lis3dshBspListener) {
                bsp->lis3dshBspListener->onInterrupt2 ();
        }
}
