#include <string>

namespace utils
{
	std::string base64_encode(const unsigned char* bytes_to_encode, unsigned int in_len);

	std::string base64_decode(const std::string& encoded_string);
}
