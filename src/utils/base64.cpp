#include "utils/base64.hpp"

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

	std::string base64_encode(const unsigned char* bytes_to_encode, unsigned int len)
	{
		std::string ret;
		unsigned char originalBytes[3];
		unsigned char convertedBytes[4];
		size_t i = 0;
		size_t j = 0;

		for (size_t i2 = 0; i2 < len; ++i2)
		{
			// Set first 3 bytes to encode
			originalBytes[i] = bytes_to_encode[i2];
			if (i == 3)
			{
				// Store left most 6 bits
				convertedBytes[0] = originalBytes[0] >> 2;
				
				// Store 2 left over bits + 4 left most bits of second byte
				convertedBytes[1] = ((originalBytes[0] & 0x3) << 4) + ((originalBytes[1] & 0xf0) >> 4);

				// Store 4 left over bits + 4 left most bits of third byte
				convertedBytes[2] = ((originalBytes[1] & 0x0f) << 2) + (originalBytes[2] >> 6);

				// Store 6 left over bits
				convertedBytes[3] = originalBytes[2] & 0x3f;

				// Get convert byte to base64 char
				for (j = 0; j < 4; ++j)
					ret += base64_chars[convertedBytes[j]];
				i = 0;
			}
			++i;
		}

		// set the left over bytes if the number of bytes is not divisible by 3
		if (i != 0)
		{
			// Zero the unused bytes
			for(j = i; j < 3; j++)
				originalBytes[j] = '\0';
			
			// Set bits as explained above
			convertedBytes[0] = originalBytes[0] >> 2;
			convertedBytes[1] = ((originalBytes[0] & 0x3) << 4) + ((originalBytes[1] & 0xf0) >> 4);
			convertedBytes[2] = ((originalBytes[1] & 0x0f) << 2) + (originalBytes[2] >> 6);
			convertedBytes[3] = originalBytes[2] & 0x3f;

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
		int len = encodedString.size();

		for (size_t i2 = 0; i2 < len && encodedString[i2] != '=' && is_base64(encodedString[i2]); ++i2)
		{
			// Get base64 chars for 4 bytes(6 bits) to decode
			encodedBytes[i++] = base64_chars.find(encodedString[i2]);
			if (i == 4)
			{
				// Get first 6 bits from first byte and last 2 bits from second byte
				decodedBytes[0] = (encodedBytes[0] << 2) + ((encodedBytes[1] & 0x30) >> 4);
				// Get last 4 bits from second byte and first 4 bits from third byte
				decodedBytes[1] = ((encodedBytes[1] & 0xf) << 4) + ((encodedBytes[2] & 0x3c) >> 2);
				// Get last 2 bits from third byte and all 6 bits from forth byte
				decodedBytes[2] = ((encodedBytes[2] & 0x3) << 6) + encodedBytes[3];

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
			decodedBytes[0] = (encodedBytes[0] << 2) + ((encodedBytes[1] & 0x30) >> 4);
			decodedBytes[1] = ((encodedBytes[1] & 0xf) << 4) + ((encodedBytes[2] & 0x3c) >> 2);
			decodedBytes[2] = ((encodedBytes[2] & 0x3) << 6) + encodedBytes[3];

			// Append remaining bytes to return string
			for (j = 0; (j < i - 1); j++)
				ret += decodedBytes[j];
		}
		return ret;
	}
}
