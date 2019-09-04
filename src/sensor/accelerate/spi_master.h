/*
 * spi_master.h
 *
 * Created: 04.05.2019 08:28:10
 *  Author: Ewa
 */ 
#include <asf.h>
#include "stdint.h"

#ifndef SPI_MASTER_H_
#define SPI_MASTER_H_

void configure_spi_master(void);
void configure_spi_master_callbacks(void);
uint8_t spi_write_register(uint8_t address, uint8_t data);
uint8_t spi_read_registers(uint8_t address, uint8_t * buf, uint8_t len);

uint8_t buf_send[100];
uint8_t buf_rec[100];
struct spi_slave_inst slave;
struct spi_module spi_master_instance;
volatile uint8_t transrev_complete_spi_master;

#endif /* SPI_MASTER_H_ */