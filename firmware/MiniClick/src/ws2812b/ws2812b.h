/* 
* ws2812b.h
*
* Created: 8/12/2023 10:01:54 AM
* Author: Andre
*/


#ifndef __WS2812B_H__
#define __WS2812B_H__
extern "C" {
	#include "asf.h"
}
#include "samd09.h"

#include "port.h"
class colorRGB{
	public:
	colorRGB(uint32_t colorHEX);
	colorRGB(uint8_t R, uint8_t G, uint8_t B);
	uint8_t R;
	uint8_t G;
	uint8_t B;
};

class ws2812b
{
//variables
public:
uint16_t num_leds;
protected:
private:

bool pixelsIsDynamic = true;
uint8_t* pixels;
uint8_t led_pin;
uint32_t lastWriteTime = 0;


//functions
public:
	ws2812b(uint16_t num_leds_, uint8_t led_pin_);
	ws2812b(uint8_t* pixels_, uint16_t num_leds_, uint8_t led_pin_);
	~ws2812b();
	void write();
	void setLEDColor(uint8_t led_num, colorRGB color);
	
	
protected:
private:
	ws2812b( const ws2812b &c );
	ws2812b& operator=( const ws2812b &c );
	void configureGPIO();
	void ws2812_write();

}; //ws2812b

#endif //__WS2812B_H__
