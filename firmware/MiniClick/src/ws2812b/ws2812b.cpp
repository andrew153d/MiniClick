/* 
* ws2812b.cpp
*
* Created: 8/12/2023 10:01:54 AM
* Author: Andre
*/


#include "ws2812b.h"

colorRGB::colorRGB(uint32_t colorHEX){
	R = (colorHEX >> 16) & 0xFF;
	G = (colorHEX >> 8) & 0xFF;
	B = colorHEX & 0xFF;
}
colorRGB::colorRGB(uint8_t R_, uint8_t G_, uint8_t B_){
	R = R_;
	G = G_;
	B = B_;
}

// default constructor
ws2812b::ws2812b(uint16_t num_leds_, uint8_t led_pin_)
{
	led_pin = led_pin_;
	pixelsIsDynamic = true;
	num_leds = num_leds_;
	configureGPIO();
	//pixels = new uint8_t[7];
} //ws2812b

ws2812b::ws2812b(uint8_t* pixels_, uint16_t num_leds_, uint8_t led_pin_){
	led_pin = led_pin_;
	num_leds = num_leds_;
	pixels = pixels_;
	pixelsIsDynamic = false;
	configureGPIO();
};

// default destructor
ws2812b::~ws2812b()
{
	if(pixelsIsDynamic)
		delete pixels;
} //~ws2812b

void ws2812b::setLEDColor(uint8_t led_num, colorRGB color){
	if(led_num>=num_leds)
		return;
	//color order is GRB
	
	pixels[led_num*3+1] = color.R;
	pixels[led_num*3] = color.G;
	pixels[led_num*3+2] = color.B;
}

void ws2812b::write(){
	ws2812_write();
}

void ws2812b::configureGPIO(){
	struct port_config config_port_pin;
	port_get_config_defaults(&config_port_pin);
	config_port_pin.direction = PORT_PIN_DIR_OUTPUT;
	port_pin_set_config(led_pin, &config_port_pin);
}

/*
	0:  HIGH: .4us
		LOW : .85us
		
	1:	HIGH: .8us
		LOW : .45us
	
	each nop statement is .125 us
	
	each clock cycle is .125us
	.4us		: .4/.125 = 3.2		-> 3 cycles = .375us
	.45us	: .45/.125 = 3.6	-> 4 cycles = 0.5us
	.8us		: .8/.125 = 6.4		-> 6 cycles = 0.75us
	.85us	: .85/.125 = 6.8	-> 7 cycles = 0.875us
	*/
void ws2812b::ws2812_write()
{
	if(pixels == NULL)
		return;
	uint16_t numBytes = num_leds*3;
	uint8_t  *ptr, *end, p, bitMask;
	uint32_t  pinMask;

	PortGroup *const port_base = port_get_group_from_gpio_pin(led_pin);
	uint32_t pin_mask  = (1UL << (led_pin % 32));
	bool level = false;
	/* Set the pin to high or low atomically based on the requested level */
	//if (level) {
	//	port_base->OUTSET.reg = pin_mask;
	//	} else {
	//	port_base->OUTCLR.reg = pin_mask;
	//}


	 volatile uint32_t *set = &(port_base->OUTSET.reg);
	 volatile uint32_t *clr = &(port_base->OUTCLR.reg);
	

	pinMask =  1ul << led_pin;
	ptr     =  pixels;
	end     =  ptr + numBytes;
	p       = *ptr++;
	bitMask =  0x80;

	

	for(;;) {
		 *set = pinMask;
		 asm("nop; nop; nop; nop; nop; nop; nop; nop;");
		 if(p & bitMask) {
			 asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
			 *clr = pinMask;
			 } else {
			 *clr = pinMask;
			 asm("nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop; nop;");
		 }
		 
		 if(bitMask >>= 1) {
			 asm("nop; nop; nop; nop;");
			 } else {
			 if(ptr >= end) break;
			 p       = *ptr++;
			 bitMask = 0x80;
		 }
	}
	//delay_cycles(2100); //this is a horrible way to do it. will change in the future
}