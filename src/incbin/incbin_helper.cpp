#include "incbin_helper.hpp"

std::string incbin_as_string(const unsigned char data[], const unsigned int size)
{
    return std::string(&data[0], (&data[0]) + size);
}