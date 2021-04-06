#include "utils/base64.hpp"

#define LAST_TWO		0xfc		// 11111100
#define FIRST_TWO		0x3f		// 00111111
#define FIRST_SIX		0x03		// 00000011
#define LAST_SIX		0xc0		// 11000000
#define LAST_FOUR		0xf0		// 11110000
#define FIRST_FOUR		0x0f		// 00001111
#define LAST_FOUR_6BIT	0x30		// 00110000
#define LAST_TWO_6BIT	0x3c		// 00111100

/*
	example:
	original/decoded bits:		111100 00|1010 0101|11 111111
	encoded bits:				111100|00 1010|0101 11|111111
*/

namespace utils
{

	static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

	static inline bool is_base64(unsigned char c) {
		return (isalnum(c) || (c == '+') || (c == '/'));
	}

	unsigned char clearBits(uint8_t byte, uint8_t mask) {
		return byte & mask;
	}

	std::string base64_encode(const unsigned char* bytes_to_encode, unsigned int len)
	{
		std::string ret;
		unsigned char originalBytes[3];
		unsigned char convertedBytes[4];
		size_t i = 0;
		size_t j = 0;

		for (size_t i2 = 0; i2 < len; ++i2)
		{
			// Set 3 bytes to encode
			originalBytes[i++] = bytes_to_encode[i2];
			if (i == 3)
			{
				// Store left most 6 bits
				convertedBytes[0] = (clearBits(originalBytes[0], LAST_TWO) >> 2);
				// Store 2 left over bits + 4 left most bits of second byte
				convertedBytes[1] = (clearBits(originalBytes[0], FIRST_SIX) << 4) + (clearBits(originalBytes[1], LAST_FOUR) >> 4);
				// Store 4 left over bits + 4 left most bits of third byte
				convertedBytes[2] = (clearBits(originalBytes[1], FIRST_FOUR) << 2) + (clearBits(originalBytes[2], LAST_SIX) >> 6);
				// Store 6 left over bits
				convertedBytes[3] = clearBits(originalBytes[2], FIRST_TWO);
				// Get base64 char and append to return string
				for (j = 0; j < 4; ++j)
					ret += base64_chars[convertedBytes[j]];
				i = 0;
			}
		}

		// set the left over bytes if the number of bytes is not divisible by 3
		if (i != 0)
		{
			// Zero the unused bytes
			for(j = i; j < 3; j++)
				originalBytes[j] = '\0';
			
			// Set bits (same as above)
			convertedBytes[0] = (clearBits(originalBytes[0], LAST_TWO) >> 2);
			convertedBytes[1] = (clearBits(originalBytes[0], FIRST_SIX) << 4) + (clearBits(originalBytes[1], LAST_FOUR) >> 4);
			convertedBytes[2] = (clearBits(originalBytes[1], FIRST_FOUR) << 2) + (clearBits(originalBytes[2], LAST_SIX) >> 6);
			convertedBytes[3] = clearBits(originalBytes[2], FIRST_TWO);

			// Get base64 char and append to return string
			for (j = 0; (j < i + 1); ++j)
				ret += base64_chars[convertedBytes[j]];

			// Unused bytes are set to '='
			while (i < 3)
			{
				ret += '=';
				++i;
			}
		}
		return ret;
	}

	std::string base64_decode(const std::string& encodedString)
	{
		std::string ret;
		unsigned char decodedBytes[3];
		unsigned char encodedBytes[4];
		size_t j = 0;
		size_t i = 0;
		size_t len = encodedString.size();

		for (size_t i2 = 0; i2 < len && encodedString[i2] != '=' && is_base64(encodedString[i2]); ++i2)
		{
			// Get base64 chars for 4 bytes(6 bits) to decode
			encodedBytes[i++] = base64_chars.find(encodedString[i2]);
			if (i == 4)
			{
				// Get first 6 bits from first byte and last 2 bits from second byte
				decodedBytes[0] = (encodedBytes[0] << 2) + (clearBits(encodedBytes[1], LAST_FOUR_6BIT) >> 4);
				// Get last 4 bits from second byte and first 4 bits from third byte
				decodedBytes[1] = (clearBits(encodedBytes[1], FIRST_FOUR) << 4) + (clearBits(encodedBytes[2], LAST_TWO_6BIT) >> 2);
				// Get last 2 bits from third byte and all 6 bits forth byte
				decodedBytes[2] = (clearBits(encodedBytes[2], FIRST_SIX) << 6) + encodedBytes[3];

				// Append converted bits to return string
				for (i = 0; (i < 3); i++)
					ret += decodedBytes[i];

				i = 0;
			}
		}
		
		// set the left over bytes if the number of bytes is not divisible by 4
		if (i != 0)
		{
			// Zero the unused bytes
			for (j = i; j < 4; j++)
				encodedBytes[j] = 0;

			// Set bits (same as above)
			decodedBytes[0] = (encodedBytes[0] << 2) + (clearBits(encodedBytes[1], LAST_FOUR_6BIT) >> 4);
			decodedBytes[1] = (clearBits(encodedBytes[1], FIRST_FOUR) << 4) + (clearBits(encodedBytes[2], LAST_TWO_6BIT) >> 2);
			decodedBytes[2] = (clearBits(encodedBytes[2], FIRST_SIX) << 6) + encodedBytes[3];

			// Append remaining bytes to return string
			for (j = 0; (j < i - 1); j++)
				ret += decodedBytes[j];
		}
		return ret;
	}
}
