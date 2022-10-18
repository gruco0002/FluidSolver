#pragma once

#include <string>
#include <vector>

namespace LibFluid::Base64 {

    std::string encode_to_base64(const std::vector<uint8_t>& data);

}