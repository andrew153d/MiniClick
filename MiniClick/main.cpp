/*
 * MiniSticks.cpp
 *
 * Created: 7/18/2023 8:15:58 PM
 * Author : Andre
 */ 



extern "C" {
	#include "asf.h"
}

#include "port.h"
//#define LED_0_PIN 22
#define NEO_PIN 3

/*
	0:  HIGH: .4us
		LOW : .85us
		
	1:	HIGH: .8us
		LOW : .45us
	
	each nop statement is .125 us
	
	each clock cycle is .125us
	.4us	: .4/.125 = 3.2		-> 3 cycles = .375us
	.45us	: .45/.125 = 3.6	-> 4 cycles = 0.5us
	.8us	: .8/.125 = 6.4		-> 6 cycles = 0.75
	.85us	: .85/.125 = 6.8	-> 7 cycles = 0.875
	*/

PortGroup *const port_base = port_get_group_from_gpio_pin(NEO_PIN);
uint8_t pinMask =  1ul << NEO_PIN;
volatile uint32_t *set = &(port_base->OUTSET.reg);
volatile uint32_t *clr = &(port_base->OUTCLR.reg);

inline void writeBit(bool bit) __attribute__((always_inline));

void writeBit(bool bit){
	//every *set = pinmask takes three clock cycles
	if(bit){
		
		*set = pinMask;
		//delay 0.8us
		asm("nop; nop; nop;");
		*clr = pinMask;
		//delay 0.45us
		asm("nop;");
		}else{
		*set = pinMask;
		//delay 0.4us
		*clr = pinMask;
		//delay 0.85us
		asm("nop; nop; nop; nop; nop;");
	}
}
void neopix_show_400k(uint32_t pin, uint8_t *pixels, uint16_t numBytes)
{

	uint8_t  *ptr, *end, p, bitMask;
	uint32_t  pinMask;

	
	uint32_t pin_mask  = (1UL << (pin % 32));
	bool level = false;
	/* Set the pin to high or low atomically based on the requested level */
	//if (level) {
	//	port_base->OUTSET.reg = pin_mask;
	//	} else {
	//	port_base->OUTCLR.reg = pin_mask;
	//}



	

	pinMask =  1ul << pin;
	ptr     =  pixels;
	end     =  ptr + numBytes;
	p       = *ptr++;
	bitMask =  0x80;

	

	/*for(ptr = pixels;ptr<;) {
		 *set = pinMask;
		 asm("nop; nop; nop; nop; nop; nop;");
		 if(p & bitMask) {
			 asm("nop; nop; nop; nop; nop; nop; nop; nop;"
			 "nop; nop; nop; nop; nop; nop; nop; nop;");
			 *clr = pinMask;
			 } else {
			 *clr = pinMask;
			 asm("nop; nop; nop; nop; nop; nop; nop; nop;"
			 "nop; nop; nop; nop; nop; nop; nop; nop;");
		 }
		 if(bitMask >>= 1) {
			 asm("nop; nop; nop; nop; nop; nop; nop;");
			 } else {
			 if(ptr >= end) break;
			 p       = *ptr++;
			 bitMask = 0x80;
		 }
	}*/
	
}
bool state = true;

#define DATA_LENGTH 10
static uint8_t write_buffer[DATA_LENGTH] = {
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
};
static uint8_t read_buffer [DATA_LENGTH];
#define SLAVE_ADDRESS 0x65
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
	
	
	
// 	system_clock_source_osc8m_config clock_config;
// 	system_clock_source_osc8m_get_config_defaults(&clock_config);
// 	clock_config.prescaler = SYSTEM_OSC8M_DIV_8;
// 	system_clock_source_osc8m_set_config(&clock_config);
	
	
	
	delay_init();
	system_gclk_init();
	
	//I2C setup
	configure_i2c_slave();
	configure_i2c_slave_callbacks();
	
		
	
	//struct port_config config_port_pin;
	//port_get_config_defaults(&config_port_pin);
	//config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	//port_pin_set_config(LED_0_PIN, &config_port_pin);
	//port_pin_set_config(NEO_PIN, &config_port_pin);
	
	/* Replace with your application code */
	
	while (1)
	{
		
		
// 		port_pin_set_output_level(LED_0_PIN, state);
// 	    state = !state;
// 		delay_cycles_ms(1000);

	}
}
