#include "utils/itos.hpp"

std::string		utils::itos(int nb)
{
	if (nb == 0)
		return "";
	return itos(nb / 10) + (char)(nb % 10 + '0');
}
