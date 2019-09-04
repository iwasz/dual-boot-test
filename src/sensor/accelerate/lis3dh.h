/*
 * lis3dh.h
 *
 * Created: 03.05.2019 17:24:58
 *  Author: Ewa
 */ 
#include "stdint.h"
#include "spi_master.h"

#ifndef LIS3DH_H_
#define LIS3DH_H_

#define LIS3DH_write(addr)		addr & 0x7f
#define LIS3DH_read(addr)		addr | 0x80	
#define LIS3DH_increment(addr)	addr | 0x40

#define LIS3DH_STATUS_REG_AUX	0x07
#define LIS3DH_OUT_ADC1_L 		0x08
#define LIS3DH_OUT_ADC1_H 		0x09
#define LIS3DH_OUT_ADC2_L 		0x0A 
#define LIS3DH_OUT_ADC2_H 		0x0B 
#define LIS3DH_OUT_ADC3_L 		0x0C 
#define LIS3DH_OUT_ADC3_H 		0x0D 
#define LIS3DH_WHO_AM_I 		0x0F 
#define LIS3DH_CTRL_REG0 		0x1E 
#define LIS3DH_TEMP_CFG_REG 	0x1F 
#define LIS3DH_CTRL_REG1 		0x20
#define LIS3DH_CTRL_REG2 		0x21
#define LIS3DH_CTRL_REG3 		0x22
#define LIS3DH_CTRL_REG4 		0x23
#define LIS3DH_CTRL_REG5 		0x24
#define LIS3DH_CTRL_REG6 		0x25
#define LIS3DH_REFERENCE 		0x26
#define LIS3DH_STATUS_REG 		0x27
#define LIS3DH_OUT_X_L 			0x28
#define LIS3DH_OUT_X_H			0x29
#define LIS3DH_OUT_Y_L			0x2A 
#define LIS3DH_OUT_Y_H			0x2B 
#define LIS3DH_OUT_Z_L			0x2C 
#define LIS3DH_OUT_Z_H			0x2D 
#define LIS3DH_FIFO_CTRL_REG 	0x2E 
#define LIS3DH_FIFO_SRC_REG 	0x2F 
#define LIS3DH_INT1_CFG 		0x30
#define LIS3DH_INT1_SRC 		0x31
#define LIS3DH_INT1_THS 		0x32
#define LIS3DH_INT1_DURATION 	0x33
#define LIS3DH_INT2_CFG 		0x34
#define LIS3DH_INT2_SRC			0x35
#define LIS3DH_INT2_THS 		0x36
#define LIS3DH_INT2_DURATION 	0x37
#define LIS3DH_CLICK_CFG		0x38
#define LIS3DH_CLICK_SRC		0x39
#define LIS3DH_CLICK_THS		0x3A 
#define LIS3DH_TIME_LIMIT		0x3B 
#define LIS3DH_TIME_LATENCY 	0x3C 
#define LIS3DH_TIME_WINDOW		0x3D 
#define LIS3DH_ACT_THS			0x3E 
#define LIS3DH_ACT_DUR			0x3F 

//STATUS_REG_AUX (07h)
#define LIS3DH_321OR			0x80
#define LIS3DH_3OR				0x40
#define LIS3DH_2OR				0x20
#define LIS3DH_1OR				0x10
#define LIS3DH_321DA			0x08
#define LIS3DH_3DA				0x04
#define LIS3DH_2DA				0x02
#define LIS3DH_1DA				0x01

//CTRL_REG0 (1Eh)
#define LIS3DH_SDO_PU_CONN		0x10
#define LIS3DH_SDO_PU_DISC		0x90

//TEMP_CFG_REG (1Fh)
#define LIS3DH_TEMP_EN			0x40
#define LIS3DH_ADC_EN			0x80

//CTRL_REG1 (20h)
#define LIS3DH_RATE_POWER_DOWN	0x00
#define LIS3DH_RATE_1Hz			0x10
#define LIS3DH_RATE_10Hz		0x20
#define LIS3DH_RATE_25Hz		0x30
#define LIS3DH_RATE_50Hz		0x40
#define LIS3DH_RATE_100Hz		0x50
#define LIS3DH_RATE_200Hz		0x60
#define LIS3DH_RATE_400Hz		0x70
#define LIS3DH_RATE_1_6kHz		0x80
#define LIS3DH_RATE_HIGH		0x90
#define LIS3DH_X_EN				0x01
#define LIS3DH_Y_EN				0x02
#define LIS3DH_Z_EN				0x03
#define LIS3DH_LP_EN			0x04

//CTRL_REG2 (21h)
#define LIS3DH_HPM_NORMAL_RST	0x00
#define LIS3DH_HPM_REF			0x40
#define LIS3DH_HPM_NORMAL		0x80
#define LIS3DH_HPM_AUTORESET	0xC0
#define LIS3DH_HPCF_1_50		0x00
#define LIS3DH_HPCF_1_100		0x10
#define LIS3DH_HPCF_1_200		0x20
#define LIS3DH_HPCF_1_500		0x30
#define LIS3DH_FDS				0x08
#define LIS3DH_HPCLICK			0x04
#define LIS3DH_HP_IA_2			0x02
#define LIS3DH_HP_IA_1			0x01

//CTRL_REG3 (22h)
#define LIS3DH_I1_CLICK 		0x80
#define LIS3DH_I1_IA1 			0x40
#define LIS3DH_I1_IA2 			0x20
#define LIS3DH_I1_ZYXDA 		0x10
#define LIS3DH_I1_321DA 		0x08
#define LIS3DH_I1_WTM 			0x04
#define LIS3DH_I1_OVERRUN 		0x02

//CTRL_REG4 (23h)
#define LIS3DH_BDU		 		0x80
#define LIS3DH_BLE	 			0x40
#define LIS3DH_FS_16G 			0x30
#define LIS3DH_FS_08G 			0x20
#define LIS3DH_FS_04G 			0x10
#define LIS3DH_FS_02G 			0x00
#define LIS3DH_HR		 		0x08
#define LIS3DH_ST_1		 		0x04
#define LIS3DH_ST_0 			0x03
#define LIS3DH_SIM		 		0x01

//CTRL_REG5 (24h)
#define LIS3DH_BOOT		 		0x80
#define LIS3DH_FIFO_EN		 	0x40
#define LIS3DH_LIR_INT1		 	0x08
#define LIS3DH_D4D_INT1		 	0x04
#define LIS3DH_LIR_INT2		 	0x02
#define LIS3DH_D4D_INT2		 	0x01

//CTRL_REG6 (25h)
#define LIS3DH_I2_CLICK		 	0x80
#define LIS3DH_I2_IA1		 	0x40
#define LIS3DH_I2_IA2		 	0x20
#define LIS3DH_I2_BOOT		 	0x10
#define LIS3DH_I2_ACT		 	0x08
#define LIS3DH_INT_POLARITY		0x02

//STATUS_REG (27h)
#define LIS3DH_ZYXOR		 	0x80
#define LIS3DH_ZOR		 		0x40
#define LIS3DH_YOR		 		0x20
#define LIS3DH_XOR		 		0x10
#define LIS3DH_ZYXDA		 	0x08
#define LIS3DH_ZDA		 		0x04
#define LIS3DH_YDA		 		0x02
#define LIS3DH_XDA		 		0x01

//FIFO_CTRL_REG (2Eh)
#define LIS3DH_FM_BYPASS		0x00
#define LIS3DH_FM_FIFO			0x40
#define LIS3DH_FM_STREAM		0x80
#define LIS3DH_FM_ST_FIFO		0xc0
#define LIS3DH_TR		 		0x20

//FIFO_SRC_REG (2Fh)
#define LIS3DH_WTM				0x80
#define LIS3DH_OVRN_FIFO	 	0x40
#define LIS3DH_EMPTY		 	0x20

//INT1_CFG (30h) INT2_CFG (34h)
#define LIS3DH_AOI		 		0x80
#define LIS3DH_6D				0x40
#define LIS3DH_ZHIE				0x20
#define LIS3DH_ZLIE				0x10
#define LIS3DH_YHIE				0x08
#define LIS3DH_YLIE				0x04
#define LIS3DH_XHIE				0x02
#define LIS3DH_XLIE				0x01

//INT1_SRC (31h) INT2_SRC (35h)
#define LIS3DH_IA				0x40
#define LIS3DH_ZH				0x20
#define LIS3DH_ZL				0x10
#define LIS3DH_YH				0x08
#define LIS3DH_YL				0x04
#define LIS3DH_XH				0x02
#define LIS3DH_XL				0x01

//CLICK_CFG (38h)
#define LIS3DH_ZD			 	0x20
#define LIS3DH_ZS			 	0x10
#define LIS3DH_YD			 	0x08
#define LIS3DH_YS			 	0x04
#define LIS3DH_XD			 	0x02
#define LIS3DH_XS			 	0x01

//CLICK_SRC (39h)
#define LIS3DH_IA				0x40
#define LIS3DH_DCLICK		 	0x20
#define LIS3DH_SCLICK		 	0x10
#define LIS3DH_Sign				0x08
#define LIS3DH_Z				0x04
#define LIS3DH_Y				0x02
#define LIS3DH_X				0x01

//CLICK_THS (3Ah)
#define LIS3DH_LIR_Click	 	0x80

typedef struct LIS3DH_CONFIG 
{
uint8_t CTRL_REG0;
uint8_t TEMP_CFG_REG;
uint8_t CTRL_REG1;
uint8_t CTRL_REG2;
uint8_t CTRL_REG3;
uint8_t CTRL_REG4;
uint8_t CTRL_REG5;
uint8_t CTRL_REG6;
uint8_t REFERENCE;
uint8_t FIFO_CTRL_REG;
uint8_t FIFO_SRC_REG;
uint8_t INT1_CFG;
uint8_t INT1_SRC;
uint8_t INT1_THS;
uint8_t INT1_DURATION;
uint8_t INT2_CFG;
uint8_t INT2_SRC;
uint8_t INT2_THS;
uint8_t INT2_DURATION;
uint8_t CLICK_CFG;
uint8_t CLICK_SRC;
uint8_t CLICK_THS;
uint8_t TIME_LIMIT;
uint8_t TIME_LATENCY;
uint8_t TIME_WINDOW;
uint8_t ACT_THS;
uint8_t ACT_DUR;
}Lis3dh_Config;
	
typedef struct LIS3DH_VALUES
{
	int16_t X;
	int16_t Y;
	int16_t Z;
}xyz;
	
uint8_t LIS3DH_INIT (struct LIS3DH_CONFIG *);
uint8_t LIS3DH_GET_XYZ (struct LIS3DH_VALUES *);
void LIS3DH_DEFAULTS(struct LIS3DH_CONFIG *);


#endif /* LIS3DH_H_ */