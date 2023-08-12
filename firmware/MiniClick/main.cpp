/*
 * MiniSticks.cpp
 *
 * Created: 7/18/2023 8:15:58 PM
 * Author : Andre
 */ 


#include "circularBuffer/circularBuffer.h"
extern "C" {
	#include "asf.h"
}
#include "samd09.h"
#include <string.h>
#include "port.h"
#include "ws2812b/ws2812b.h"
#define NEO_PIN 10


const int BUTTON_PIN_1 = 16;
const int BUTTON_PIN_2 = 17;
const int BUTTON_PIN_3 = 22;
const int BUTTON_PIN_4 = 23;
const int LED_PIN = 27;

struct buttons{
	bool button_1;
	bool button_2;
	bool button_3;
	bool button_4;
	}last_button_level, current_button_level;

#define BUTTON_PRESSED 0x80
#define BUTTON_RELEASED 0x40
#define CLICK_QUEUE_SIZE_POSITION 0x00
#define CLICK_QUEUE_HEAD_POSITION 0x01
#define ADDRESS_BASE 0x50

uint8_t device_registers[10];
CircularBuffer buffer(10);

static uint8_t read_buffer[2];
#define SLAVE_ADDRESS 0x12
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
	volatile uint8_t read_address = packet.data[0] & 0x0F;
	 packet.data_length = 1;
	switch(read_address){
		case 0x00:
		device_registers[read_address] = buffer.size();
		 packet.data = &device_registers[read_address];
		break;
		case 0x01:
		 buffer.read(device_registers[read_address]);
		 packet.data = &device_registers[read_address];
		break;
	}	
	/* Write buffer to master */
	i2c_slave_write_packet_job(module, &packet);
}

void i2c_write_request_callback(struct i2c_slave_module *const module)
{
	/* Init i2c packet */
	packet.data_length = 1;
	packet.data        = read_buffer;
	/* Read buffer from master */
	if (i2c_slave_read_packet_job(module, &packet) != STATUS_OK) {
		//packet.data        = read_buffer;
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

void configure_gpio_pins()
{
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_INPUT;
	config_port_pin.input_pull = PORT_PIN_PULL_UP;
	port_pin_set_config(BUTTON_PIN_1, &config_port_pin);
	port_pin_set_config(BUTTON_PIN_2, &config_port_pin);
	port_pin_set_config(BUTTON_PIN_3, &config_port_pin);
	port_pin_set_config(BUTTON_PIN_4, &config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_PIN, &config_port_pin);
}

void configure_i2c_pinmux()
{
	struct system_pinmux_config config_pinmux;
	system_pinmux_get_config_defaults(&config_pinmux);
	config_pinmux.mux_position = 1 << 1;
	system_pinmux_pin_set_config(14, &config_pinmux);
	system_pinmux_pin_set_config(15, &config_pinmux);
}

int main(void)
{
	/* Initialize the SAM system */
	SystemInit();
	system_clock_init();
	
	//system_clock_source_osc8m_config clock_config;
	//system_clock_source_osc8m_get_config_defaults(&clock_config);
	//clock_config.prescaler = SYSTEM_OSC8M_DIV_1;
	//system_clock_source_osc8m_set_config(&clock_config);
	 
	
	delay_init();
	
	
	
	
	//set pins for GPIO input
	configure_gpio_pins();
	
	//set pinmux for I2C lines
	 configure_i2c_pinmux();
	 
	//I2C setup
	configure_i2c_slave();
	configure_i2c_slave_callbacks();
	
	uint8_t pixels[4*3];
	uint32_t color = 0;
	memset(pixels, 0, sizeof(pixels));
	ws2812b ledStrip(&pixels[0], 4, NEO_PIN);
	ledStrip.write();
	//ledStrip.setLEDColor(0, colorRGB(0xEEA255));
	//ledStrip.setLEDColor(2, colorRGB(0x0011FF));
	
	
	while (1)
	{
		
		
		volatile uint32_t clk_speed = system_gclk_gen_get_hz(0);
		last_button_level = current_button_level;
		current_button_level.button_1 = port_pin_get_input_level(BUTTON_PIN_1);
		current_button_level.button_2 = port_pin_get_input_level(BUTTON_PIN_2);
		current_button_level.button_3 = port_pin_get_input_level(BUTTON_PIN_3);
		current_button_level.button_4 = port_pin_get_input_level(BUTTON_PIN_4);
		
		if(!current_button_level.button_1 && last_button_level.button_1){
			buffer.write(0x01 | BUTTON_PRESSED);
			port_pin_set_output_level(LED_PIN, true);
			ledStrip.setLEDColor(0, colorRGB(0xFF8000));
			
		}
		if(!current_button_level.button_2 && last_button_level.button_2){
			buffer.write(0x02 | BUTTON_PRESSED);
			port_pin_set_output_level(LED_PIN, true);
			ledStrip.setLEDColor(1, colorRGB(0xFF8000));
		}
		if(!current_button_level.button_3 && last_button_level.button_3){
			buffer.write(0x03 | BUTTON_PRESSED);
			port_pin_set_output_level(LED_PIN, true);
			ledStrip.setLEDColor(2, colorRGB(0xFF8000));
		}
		if(!current_button_level.button_4 && last_button_level.button_4){
			buffer.write(0x04 | BUTTON_PRESSED);
			port_pin_set_output_level(LED_PIN, true);
			ledStrip.setLEDColor(3, colorRGB(0xFF8000));
		}
		if(current_button_level.button_1 && !last_button_level.button_1){
			buffer.write(0x01 | BUTTON_RELEASED);
			port_pin_set_output_level(LED_PIN, false);
			ledStrip.setLEDColor(0, colorRGB(0x000000));
		}
		if(current_button_level.button_2 && !last_button_level.button_2){
			buffer.write(0x02 | BUTTON_RELEASED);
			port_pin_set_output_level(LED_PIN, false);
			ledStrip.setLEDColor(1, colorRGB(0x000000));
		}
		if(current_button_level.button_3 && !last_button_level.button_3){
			buffer.write(0x03 | BUTTON_RELEASED);
			port_pin_set_output_level(LED_PIN, false);
			ledStrip.setLEDColor(2, colorRGB(0x000000));
		}
		if(current_button_level.button_4 && !last_button_level.button_4){
			buffer.write(0x04 | BUTTON_RELEASED);
			port_pin_set_output_level(LED_PIN, false);
			ledStrip.setLEDColor(3, colorRGB(0x000000));
		}
		delay_cycles_ms(10);
		ledStrip.write();
		
		
	}
}
