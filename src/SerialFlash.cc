/****************************************************************************
 *                                                                          *
 *  Author : lukasz.iwaszkiewicz@gmail.com                                  *
 *  ~~~~~~~~                                                                *
 *  License : see COPYING file for details.                                 *
 *  ~~~~~~~~~                                                               *
 ****************************************************************************/

#include "SerialFlash.h"
#include "Debug.h"
#include "Timer.h"

/*****************************************************************************/

void SerialFlash::read (uint32_t address, uint8_t *buf, size_t len)
{
        waitUntilReady ();
        spi->setNss (false);
        uint8_t tmp[4];
        prepareAddress (READ, address, tmp);
        spi->transmit8 (tmp, 4);
        HAL_Delay (10);
        spi->receive8 (buf, len);
        spi->setNss (true);
        waitUntilReady ();
}

/*****************************************************************************/

void SerialFlash::write (uint32_t address, uint8_t const *buf, size_t len)
{
        writeEnable (); // Every (mutating?) command disables writing
        waitUntilWriteEnabled ();

        spi->setNss (false);
        uint8_t tmp[4] = { 0 };
        prepareAddress (PAGE_PROGRAM, address, tmp);
        spi->transmit8 (tmp, 4);
        spi->transmit8 (buf, len);
        spi->setNss (true);

        waitUntilReady ();
}

/*****************************************************************************/

uint8_t SerialFlash::statusRegisterRead () const
{
        spi->setNss (false);
        spi->transmit8 (STATUS_REGISTER_READ);
        uint8_t ret = spi->transmit8 (0xff);
        spi->setNss (true);
        return ret;
}

/*****************************************************************************/

void SerialFlash::statusRegisterWrite (uint8_t value)
{
        spi->setNss (false);
        spi->transmit8 (STATUS_REGISTER_WRITE);
        spi->transmit8 (value);
        spi->setNss (true);
}

/*****************************************************************************/

uint32_t SerialFlash::readSiliconId1 () const
{
        spi->setNss (false);
        spi->transmit8 (READ_SILICON_ID1);
        spi->transmit8 (0xff);
        uint32_t ret = static_cast<uint32_t> (spi->transmit8 (0xff));
        ret |= static_cast<uint32_t> (spi->transmit8 (0xff)) << 8;
        ret |= static_cast<uint32_t> (spi->transmit8 (0xff)) << 16;
        spi->setNss (true);
        return ret;
}

/*****************************************************************************/

void SerialFlash::writeEnable ()
{
        spi->setNss (false);
        spi->transmit8 (WRITE_ENABLE);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::writeDisable ()
{
        spi->setNss (false);
        spi->transmit8 (WRITE_DISABLE);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::globalBlockProtectionUnlock ()
{
        spi->setNss (false);
        spi->transmit8 (GLOBAL_BLOCK_PROTECTION_UNLOCK);
        spi->setNss (true);
        writeDisable ();
}

/*****************************************************************************/

void SerialFlash::chipErase ()
{
        writeEnable (); // Every (mutating?) command disables writing
        waitUntilWriteEnabled ();

        spi->setNss (false);
        spi->transmit8 (CHIP_ERASE);
        spi->setNss (true);

        waitUntilReady ();
        writeDisable ();
}

/*****************************************************************************/

uint8_t SerialFlash::configRegisterRead () const
{
        spi->setNss (false);
        spi->transmit8 (CONFIGURATION_REGISTER_READ);
        uint8_t ret = spi->transmit8 (0xff);
        spi->setNss (true);
        return ret;
}

/*****************************************************************************/

void SerialFlash::blockProtectionRegisterRead (BlockProtectionRegister &reg) const
{
        spi->setNss (false);
        spi->transmit8 (BLOCK_PROTECTION_REGISTER_READ);
        spi->receive8 (reg, 10);
        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::sectorErase (uint32_t address, uint8_t eraseInstruction)
{
        writeEnable (); // Every (mutating?) command disables writing
        waitUntilWriteEnabled ();

        uint8_t tmp[4];
        prepareAddress (eraseInstruction, address, tmp);

        spi->setNss (false);
        spi->transmit8 (tmp, 4, nullptr);
        spi->setNss (true);

        waitUntilReady ();
}

/*****************************************************************************/

void SerialFlash::powerDown (bool off)
{
        spi->setNss (false);

        if (off) {
                spi->transmit8 (POWER_DOWN);
        }
        else {
                spi->transmit8 (EXIT_POWER_DOWN_MODE);
        }

        spi->setNss (true);
}

/*****************************************************************************/

void SerialFlash::waitUntilReady () const
{
        while (statusRegisterRead () & BUSY)
                ;
}

/*****************************************************************************/

void SerialFlash::waitUntilWriteEnabled () const
{
        while (!(statusRegisterRead () & WEN))
                ;
}

/*****************************************************************************/

void SerialFlash::prepareAddress (uint8_t command, uint32_t address, uint8_t outData[4])
{
        outData[0] = command;
        outData[1] = (address & 0x00ff0000) >> 16;
        outData[2] = (address & 0x0000ff00) >> 8;
        outData[3] = (address & 0x000000ff) >> 0;
}
