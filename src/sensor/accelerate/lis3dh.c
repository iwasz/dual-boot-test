/*
 * lis3dh.c
 *
 * Created: 03.05.2019 17:24:13
 *  Author: Remi
 */ 
#include "lis3dh.h"

uint8_t LIS3DH_INIT (struct LIS3DH_CONFIG * conf)
{
	configure_spi_master();
	configure_spi_master_callbacks();	
	spi_write_register(LIS3DH_CTRL_REG0,conf->CTRL_REG0);
	spi_write_register(LIS3DH_TEMP_CFG_REG,conf->TEMP_CFG_REG);
	spi_write_register(LIS3DH_CTRL_REG1,conf->CTRL_REG1);
	spi_write_register(LIS3DH_CTRL_REG2,conf->CTRL_REG2);
	spi_write_register(LIS3DH_CTRL_REG3,conf->CTRL_REG3);
	spi_write_register(LIS3DH_CTRL_REG4,conf->CTRL_REG4);
	spi_write_register(LIS3DH_CTRL_REG5,conf->CTRL_REG5);
	spi_write_register(LIS3DH_CTRL_REG6,conf->CTRL_REG6);
	spi_write_register(LIS3DH_REFERENCE,conf->REFERENCE);
	spi_write_register(LIS3DH_FIFO_CTRL_REG,conf->FIFO_CTRL_REG);
	spi_write_register(LIS3DH_FIFO_SRC_REG,conf->FIFO_SRC_REG);
	spi_write_register(LIS3DH_INT1_CFG,conf->INT1_CFG);
	spi_write_register(LIS3DH_INT1_SRC,conf->INT1_SRC);
	spi_write_register(LIS3DH_INT1_THS,conf->INT1_THS);
	spi_write_register(LIS3DH_INT1_DURATION,conf->INT1_DURATION);
	spi_write_register(LIS3DH_INT2_CFG,conf->INT2_CFG);
	spi_write_register(LIS3DH_INT2_SRC,conf->INT2_SRC);
	spi_write_register(LIS3DH_INT2_THS,conf->INT2_THS);
	spi_write_register(LIS3DH_INT2_DURATION,conf->INT2_DURATION);
	spi_write_register(LIS3DH_CLICK_CFG,conf->CLICK_CFG);
	spi_write_register(LIS3DH_CLICK_SRC,conf->CLICK_SRC);
	spi_write_register(LIS3DH_CLICK_THS,conf->CLICK_THS);
	spi_write_register(LIS3DH_TIME_LIMIT,conf->TIME_LIMIT);
	spi_write_register(LIS3DH_TIME_LATENCY,conf->TIME_LATENCY);
	spi_write_register(LIS3DH_TIME_WINDOW,conf->TIME_WINDOW);
	spi_write_register(LIS3DH_ACT_THS,conf->ACT_THS);
	spi_write_register(LIS3DH_ACT_DUR,conf->ACT_DUR);
	
	
	return 1;
}

uint8_t LIS3DH_GET_XYZ (struct LIS3DH_VALUES * values)
{
	uint8_t buf[7];
	spi_read_registers(LIS3DH_increment(LIS3DH_read(LIS3DH_OUT_X_L)), buf, sizeof(buf));
	values->X=buf[2];
	values->X=values->X<<8;
	values->X+=buf[1];
	values->Y=buf[4];
	values->Y=values->Y<<8;
	values->Y+=buf[3];
	values->Z=buf[6];
	values->Z=values->Z<<8;
	values->Z+=buf[5];
	
	return 1;
}

void LIS3DH_DEFAULTS(struct LIS3DH_CONFIG *conf)
{
	conf->CTRL_REG0=0x10;
	conf->TEMP_CFG_REG=0x00;
	conf->CTRL_REG1=0x07;
	conf->CTRL_REG2=0x00;
	conf->CTRL_REG3=0x00;
	conf->CTRL_REG4=0x00;
	conf->CTRL_REG5=0x00;
	conf->CTRL_REG6=0x00;
	conf->REFERENCE=0x00;
	conf->FIFO_CTRL_REG=0x00;
	conf->FIFO_SRC_REG=0x00;
	conf->INT1_CFG=0x00;
	conf->INT1_SRC=0x00;
	conf->INT1_THS=0x00;
	conf->INT1_DURATION=0x00;
	conf->INT2_CFG=0x00;
	conf->INT2_SRC=0x00;
	conf->INT2_THS=0x00;
	conf->INT2_DURATION=0x00;
	conf->CLICK_CFG=0x00;
	conf->CLICK_SRC=0x00;
	conf->CLICK_THS=0x00;
	conf->TIME_LIMIT=0x00;
	conf->TIME_LATENCY=0x00;
	conf->TIME_WINDOW=0x00;
	conf->ACT_THS=0x00;
	conf->ACT_DUR=0x00;
}