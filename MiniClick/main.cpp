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
#define LED_0_PIN 2
#define NEO_PIN 5

/*
	0:  HIGH: .4us
		LOW : .85us
		
	1:	HIGH: .8us
		LOW : .45us
	
	each nop statement is .125 us
	
	each clock cycle is .125us
	.4us		: .4/.125 = 3.2		-> 3 cycles = .375us
	.45us	: .45/.125 = 3.6	-> 4 cycles = 0.5us
	.8us		: .8/.125 = 6.4		-> 6 cycles = 0.75
	.85us	: .85/.125 = 6.8	-> 7 cycles = 0.875
	*/


void neopix_show_400k(uint32_t pin, uint8_t *pixels, uint16_t numBytes)
{

	uint8_t  *ptr, *end, p, bitMask;
	uint32_t  pinMask;

	PortGroup *const port_base = port_get_group_from_gpio_pin(pin);
	uint32_t pin_mask  = (1UL << (pin % 32));
	bool level = false;
	/* Set the pin to high or low atomically based on the requested level */
	//if (level) {
	//	port_base->OUTSET.reg = pin_mask;
	//	} else {
	//	port_base->OUTCLR.reg = pin_mask;
	//}


	 volatile uint32_t *set = &(port_base->OUTSET.reg);
	 volatile uint32_t *clr = &(port_base->OUTCLR.reg);
	

	pinMask =  1ul << pin;
	ptr     =  pixels;
	end     =  ptr + numBytes;
	p       = *ptr++;
	bitMask =  0x80;

	

	for(;;) {
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
	}
}



int main(void)
{
	/* Initialize the SAM system */
	SystemInit();
	
	
	
	system_clock_source_osc8m_config clock_config;
	system_clock_source_osc8m_get_config_defaults(&clock_config);
	clock_config.prescaler = SYSTEM_OSC8M_DIV_1;
	system_clock_source_osc8m_set_config(&clock_config);
	
	
	
	delay_init();
	system_gclk_init();
	
	int i;
	bool state = true;
	
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(LED_0_PIN, &config_port_pin);
	port_pin_set_config(NEO_PIN, &config_port_pin);
	
	
	
	
	/* Replace with your application code */
	uint8_t pixels[12];
	pixels[0] = 100;
	while (1)
	{
		neopix_show_400k(NEO_PIN, &pixels[0], 12);
		port_pin_set_output_level(LED_0_PIN, state);
		state = !state;
		delay_cycles_ms(1000);
	}
}
