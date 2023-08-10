/*
 * circularBuffer.cpp
 *
 * Created: 8/4/2023 3:36:54 PM
 *  Author: Andre
 */ 
#include "circularBuffer.h"

CircularBuffer::CircularBuffer(uint8_t size) : capacity(size), readIndex(0), writeIndex(0) {
	//buffer = new uint8_t[size];
}

CircularBuffer::~CircularBuffer() {
	//delete[] buffer;
}

uint8_t CircularBuffer::size(){
if (writeIndex >= readIndex) {
        return writeIndex - readIndex;
    } else {
        return capacity - (readIndex - writeIndex);
    }
}

bool CircularBuffer::isEmpty() const {
	return readIndex == writeIndex;
}

bool CircularBuffer::isFull() const {
	return (writeIndex + 1) % capacity == readIndex;
}

bool CircularBuffer::write(uint8_t value) {
	if (isFull()) {
		return false;
	}

	buffer[writeIndex] = value;
	writeIndex = (writeIndex + 1) % capacity;
	return true;
}

bool CircularBuffer::read(uint8_t& value) {
	if (isEmpty()) {
		return false;
	}

	value = buffer[readIndex];
	readIndex = (readIndex + 1) % capacity;
	return true;
}
