#include "Base64.hpp"
namespace LibFluid::Base64 {


    size_t get_length_of_output_string(size_t byte_length) {
        size_t result = byte_length;

        // make the input length a full multiple of three to accomodate for necessary padding
        if (result % 3 > 0) {
            result += 3 - (result % 3);
        }

        // each three input bytes are represented by four output chars
        result = result / 3 * 4;

        return result;
    }


    std::string encode_to_base64(const std::vector<uint8_t>& data) {
        const char* base64_characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        // handle the empty byte array
        if (data.empty()) {
            return "";
        }

        // determine the output length and allocate a large enough string
        auto output_length = get_length_of_output_string(data.size());
        std::string output;
        output.resize(output_length);

        size_t output_index = 0;
        for (size_t input_index = 0; input_index < data.size(); input_index += 3) {
            // combine three input bytes into a 32-bit unsigned integer, which we can more easily work with
            uint32_t triple = data[input_index];
            if (input_index + 1 < data.size()) {
                triple = triple << 8 | data[input_index + 1];
            } else {
                triple = triple << 8;
            }
            if (input_index + 2 < data.size()) {
                triple = triple << 8 | data[input_index + 2];
            } else {
                triple = triple << 8;
            }

            // lookup the corresponding characters and set them at the correct place in the output string
            output[output_index] = base64_characters[(triple >> 18) & 0x3F];
            output[output_index + 1] = base64_characters[(triple >> 12) & 0x3F];

            // the remaining two characters can either be padding characters (if we consumed all bytes) or they are encoded
            if (input_index + 1 < data.size()) {
                output[output_index + 2] = base64_characters[(triple >> 6) & 0x3F];
            } else {
                // padding
                output[output_index + 2] = '=';
            }
            if (input_index + 2 < data.size()) {
                output[output_index + 3] = base64_characters[triple & 0x3F];
            } else {
                // padding
                output[output_index + 3] = '=';
            }

            // also increase the output index
            output_index += 4;
        }

        return output;
    }
} // namespace LibFluid::Base64