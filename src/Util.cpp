#include <iostream>
#include "Util.hpp"

void Util::errorExit(const std::string &_msg)
{
	std::cerr << &_msg << '\n';
	exit(EXIT_FAILURE);
}
