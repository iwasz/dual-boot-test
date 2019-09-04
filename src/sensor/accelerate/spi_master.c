/*
 * spi_master.c
 *
 * Created: 04.05.2019 08:27:34
 *  Author: Remi
 */ 

#include "spi_master.h"

static void callback_spi_master( struct spi_module *const module);

//! [configure_spi]
void configure_spi_master(void)
{
	//! [config]
	struct spi_config config_spi_master;
	//! [config]
	//! [slave_config]
	struct spi_slave_inst_config slave_dev_config;
	//! [slave_config]
	/* Configure and initialize software device instance of peripheral slave */
	//! [slave_conf_defaults]
	spi_slave_inst_get_config_defaults(&slave_dev_config);
	//! [slave_conf_defaults]
	//! [ss_pin]
	slave_dev_config.ss_pin = SLAVE_SELECT_PIN;
	//! [ss_pin]
	//! [slave_init]
	spi_attach_slave(&slave, &slave_dev_config);
	//! [slave_init]
	/* Configure, initialize and enable SERCOM SPI module */
	//! [conf_defaults]
	spi_get_config_defaults(&config_spi_master);
	//! [conf_defaults]
	//! [mux_setting]
	config_spi_master.mux_setting = CONF_MASTER_MUX_SETTING;
	//! [mux_setting]
	config_spi_master.pinmux_pad0 = CONF_MASTER_PINMUX_PAD0;
	config_spi_master.pinmux_pad1 = CONF_MASTER_PINMUX_PAD1;
	config_spi_master.pinmux_pad2 = CONF_MASTER_PINMUX_PAD2;
	config_spi_master.pinmux_pad3 = CONF_MASTER_PINMUX_PAD3;
	config_spi_master.data_order=SPI_DATA_ORDER_MSB;
	config_spi_master.transfer_mode=SPI_TRANSFER_MODE_3;


	//! [init]
	spi_init(&spi_master_instance, CONF_MASTER_SPI_MODULE, &config_spi_master);
	//! [init]

	//! [enable]
	spi_enable(&spi_master_instance);
	//! [enable]

}
//! [configure_spi]
void configure_spi_master_callbacks(void)
{
	spi_register_callback(&spi_master_instance, callback_spi_master,
	SPI_CALLBACK_BUFFER_TRANSCEIVED);
	spi_enable_callback(&spi_master_instance, SPI_CALLBACK_BUFFER_TRANSCEIVED);
}

static void callback_spi_master( struct spi_module *const module)
{
	transrev_complete_spi_master = 1;
	port_pin_toggle_output_level(LED_GREEN);
}

uint8_t spi_write_register(uint8_t address, uint8_t data)
{
	uint8_t result=0;
	uint8_t tst=0;
	spi_select_slave(&spi_master_instance, &slave, true);
	buf_send[0]=address;
	buf_send[1]=data;
	transrev_complete_spi_master = 0;
	spi_transceive_buffer_job(&spi_master_instance, buf_send,buf_rec,2);
	tst=0;
	while (transrev_complete_spi_master==0) {
		delay_ms(1);
		tst++;
		if (tst>200)
		{
			transrev_complete_spi_master=1;
			result=1;
		}
	}
	spi_select_slave(&spi_master_instance, &slave, false);	
	return result;	
}

uint8_t spi_read_registers(uint8_t address, uint8_t * buf, uint8_t len)
{
	uint8_t tst=0;
	uint8_t buf_snd[len];
	
	spi_select_slave(&spi_master_instance, &slave, true);
	buf_snd[0]=address;
	for (uint8_t i=1;i<len;i++)
	{
		buf_snd[i]=0;
	}
	
	transrev_complete_spi_master = 0;
	spi_transceive_buffer_job(&spi_master_instance, buf_snd,buf,len);
	tst=0;
	while (transrev_complete_spi_master==0) {
		delay_ms(1);
		tst++;
		if (tst>200)
		{
			transrev_complete_spi_master=1;
		}
	}
	
	spi_select_slave(&spi_master_instance, &slave, false);	
	return 1;
}

