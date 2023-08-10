/*
 * circularBuffer.h
 *
 * Created: 8/4/2023 3:37:10 PM
 *  Author: Andre
 */ 


#ifndef CIRCULARBUFFER_H_
#define CIRCULARBUFFER_H_

#include <stdio.h>

class CircularBuffer {
	private:
	uint8_t buffer[10];
	uint8_t capacity;
	uint8_t readIndex;
	uint8_t writeIndex;

	public:
	CircularBuffer(uint8_t size);
	~CircularBuffer();

	bool isEmpty() const;
	bool isFull() const;
	bool write(uint8_t value);
	bool read(uint8_t& value);
	uint8_t size();
};



#endif /* CIRCULARBUFFER_H_ */