/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "lora_base.h"
#include "lis3dh.h"

int main (void)
{
	system_init();
	
	delay_init();
	irq_initialize_vectors();
	cpu_irq_enable();

	configure_tc();
	configure_tc_callbacks();
	system_interrupt_enable_global();
	
	udc_start();
	transrev_complete_spi_master = 0;
	
	/* Insert application code here, after the board has been initialized. */
	port_pin_set_output_level(LIS_POWER,LIS_POWER_1);
	delay_ms(100);
	
	LIS3DH_DEFAULTS(&lis3dh_conf);
	lis3dh_conf.CTRL_REG1=(LIS3DH_RATE_10Hz|LIS3DH_X_EN|LIS3DH_Y|LIS3DH_Z|LIS3DH_LP_EN);
	LIS3DH_INIT(&lis3dh_conf);
	
	while (1)
	{
		
		LIS3DH_GET_XYZ(&xyz_values);
		if (write_usb==1)
		{
			sprintf(buf_rec,"X: %d, Y: %d, Z: %d\n\r",xyz_values.X,xyz_values.Y,xyz_values.Z);
			//buf_rec[6]=0x0a;
			udi_cdc_write_buf(buf_rec,sizeof(buf_rec));	
			write_usb=0;
		}
		delay_ms(50);
	}
}





void configure_tc(void)
{
	tc_get_config_defaults(&tc_timer_conf);
	tc_timer_conf.counter_size = TC_COUNTER_SIZE_16BIT;
	tc_timer_conf.clock_source = GCLK_GENERATOR_0;
	tc_timer_conf.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	tc_timer_conf.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	tc_timer_conf.counter_16_bit.compare_capture_channel[0] = 46874;
	tc_timer_conf.pwm_channel[0].enabled = false;
	tc_init(&tc_timer, TC3, &tc_timer_conf);
	tc_enable(&tc_timer);

/*	tc_get_config_defaults(&tc_automatic_conf);
	tc_automatic_conf.counter_size = TC_COUNTER_SIZE_16BIT;
	tc_automatic_conf.clock_source = GCLK_GENERATOR_0;
	tc_automatic_conf.clock_prescaler = TC_CLOCK_PRESCALER_DIV64;
	tc_automatic_conf.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	tc_automatic_conf.counter_16_bit.compare_capture_channel[0] = 7500;
	tc_automatic_conf.pwm_channel[0].enabled = false;
	tc_init(&tc_automatic, TC1, &tc_automatic_conf);
	tc_enable(&tc_automatic);

	tc_get_config_defaults(&tc_PWM_conf);
	tc_PWM_conf.counter_size = TC_COUNTER_SIZE_8BIT;
	tc_PWM_conf.clock_source = GCLK_GENERATOR_0;
	tc_PWM_conf.clock_prescaler = TC_CLOCK_PRESCALER_DIV1;
	tc_PWM_conf.wave_generation = TC_WAVE_GENERATION_NORMAL_PWM;
	tc_PWM_conf.counter_8_bit.period = 120;
	tc_PWM_conf.counter_8_bit.compare_capture_channel[0] = 60;
	tc_PWM_conf.pwm_channel[0].enabled = true;
	tc_PWM_conf.pwm_channel[0].pin_out = PIN_PA04;
	tc_PWM_conf.pwm_channel[0].pin_mux = PORT_PMUX_PMUXE_F;
	tc_init(&tc_PWM, TC0, &tc_PWM_conf);
	tc_enable(&tc_PWM);

	tc_get_config_defaults(&tc_stepper_conf);
	tc_stepper_conf.counter_size = TC_COUNTER_SIZE_16BIT;
	tc_stepper_conf.clock_source = GCLK_GENERATOR_0;
	tc_stepper_conf.clock_prescaler = TC_CLOCK_PRESCALER_DIV1024;
	tc_stepper_conf.wave_generation = TC_WAVE_GENERATION_MATCH_FREQ;
	tc_stepper_conf.counter_16_bit.compare_capture_channel[0] = 20;
	tc_stepper_conf.pwm_channel[0].enabled = false;
	tc_init(&tc_stepper, TC2, &tc_stepper_conf);
	tc_enable(&tc_stepper);
*/

}
void configure_tc_callbacks(void)
{
	tc_register_callback(&tc_timer, my_timer,TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_timer, TC_CALLBACK_CC_CHANNEL0);
/*	tc_register_callback(&tc_automatic, automatic,TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_automatic, TC_CALLBACK_CC_CHANNEL0);
	tc_register_callback(&tc_stepper, stepper,TC_CALLBACK_CC_CHANNEL0);
	tc_enable_callback(&tc_stepper, TC_CALLBACK_CC_CHANNEL0);
*/
}


//FUNKCJA WYWO£YWANA CYKLICZNIE
void my_timer(struct tc_module *const module_inst)
{
	port_pin_toggle_output_level(LED_YELLOW);
	write_usb=1;

}