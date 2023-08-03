/*
 * MiniSticks.cpp
 *
 * Created: 7/18/2023 8:15:58 PM
 * Author : Andre
 */ 



extern "C" {
	#include "asf.h"
}



#define DATA_LENGTH 10

static uint8_t write_buffer[DATA_LENGTH] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
};
static uint8_t read_buffer [DATA_LENGTH];
#define SLAVE_ADDRESS 0x1F
struct i2c_slave_module i2c_slave_instance;
static struct i2c_slave_packet packet;
Sercom slave_instance;

void configure_i2c_slave(void)
{
	/* Initialize config structure and module instance */
	struct i2c_slave_config config_i2c_slave;
	i2c_slave_get_config_defaults(&config_i2c_slave);
	/* Change address and address_mode */
	config_i2c_slave.address      = SLAVE_ADDRESS;
	config_i2c_slave.address_mode = I2C_SLAVE_ADDRESS_MODE_MASK;
	/* Initialize and enable device with config */
	
	volatile status_code code = i2c_slave_init(&i2c_slave_instance, SERCOM0, &config_i2c_slave);
	i2c_slave_enable(&i2c_slave_instance);
}

void i2c_read_request_callback(
struct i2c_slave_module *const module)
{
	/* Init i2c packet */
	packet.data_length = DATA_LENGTH;
	packet.data        = write_buffer;
	/* Write buffer to master */
	i2c_slave_write_packet_job(module, &packet);
}

void i2c_write_request_callback(struct i2c_slave_module *const module)
{
	/* Init i2c packet */
	packet.data_length = DATA_LENGTH;
	packet.data        = read_buffer;
	/* Read buffer from master */
	if (i2c_slave_read_packet_job(module, &packet) != STATUS_OK) {
	}
}

void configure_i2c_slave_callbacks(void)
{
	/* Register and enable callback functions */
	i2c_slave_register_callback(&i2c_slave_instance, i2c_read_request_callback,
	I2C_SLAVE_CALLBACK_READ_REQUEST);
	i2c_slave_enable_callback(&i2c_slave_instance,
	I2C_SLAVE_CALLBACK_READ_REQUEST);
	i2c_slave_register_callback(&i2c_slave_instance, i2c_write_request_callback,
	I2C_SLAVE_CALLBACK_WRITE_REQUEST);
	i2c_slave_enable_callback(&i2c_slave_instance,
	I2C_SLAVE_CALLBACK_WRITE_REQUEST);
}



int main(void)
{
	/* Initialize the SAM system */
	SystemInit();
	
	 struct system_pinmux_config config_pinmux;
	 system_pinmux_get_config_defaults(&config_pinmux);
	 config_pinmux.mux_position = 1 << 1;
	 system_pinmux_pin_set_config(14, &config_pinmux);
	 system_pinmux_pin_set_config(15, &config_pinmux);
	
	//I2C setup
	configure_i2c_slave();
	configure_i2c_slave_callbacks();
	
	while (1)
	{}
}
