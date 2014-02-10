unsigned int getBitArrayIndex(unsigned int width, unsigned int x, unsigned int y)
{
	return width * y + x;
}

int getBit(unsigned char* array, unsigned int width, unsigned int x, unsigned int y)
{
	unsigned int bitArrayIndex = getBitArrayIndex(width, x, y);
	unsigned char bitmask = 0x80 >> (bitArrayIndex % 8);
	unsigned char byte = array[bitArrayIndex / 8];
	int bit = 1;

	if((bitmask & byte) == 0x00) {
		bit = 0;
	}

	return bit;
}

void setBit(unsigned char* array, unsigned int width, unsigned int x, unsigned int y, unsigned char bit)
{
	unsigned int bitArrayIndex = getBitArrayIndex(width, x, y);
	unsigned char byte = array[bitArrayIndex / 8];
	unsigned char bitmask;

	if(bit == 0) {
		bitmask = ~(0x80 >> (bitArrayIndex % 8));
		byte = byte & bitmask;
	} else {
		bitmask = 0x80 >> (bitArrayIndex % 8);
		byte = byte | bitmask;
	}

	array[bitArrayIndex / 8] = byte;
}