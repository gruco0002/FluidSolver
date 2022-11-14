#pragma once

#include <string>
#include <vector>

namespace LibFluid::Base64 {

    std::string encode_to_base_64(const std::vector<uint8_t>& data);
    std::vector<uint8_t> decode_from_base_64(const std::string& data);

} // namespace LibFluid::Base64