#pragma once

#define INCBIN_STYLE INCBIN_STYLE_SNAKE
#define INCBIN_PREFIX g_
#include <incbin/incbin.h>

#include <string>

std::string incbin_as_string(const unsigned char data[], const unsigned int size);
