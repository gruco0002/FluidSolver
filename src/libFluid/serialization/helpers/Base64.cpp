#include "Base64.hpp"
namespace LibFluid::Base64
{

    size_t get_length_of_output_string(size_t byte_length)
    {
        size_t result = byte_length;

        // make the input length a full multiple of three to accommodate for necessary padding
        if (result % 3 > 0)
        {
            result += 3 - (result % 3);
        }

        // each three input bytes are represented by four output chars
        result = result / 3 * 4;

        return result;
    }

    std::string encode_to_base_64(const std::vector<uint8_t> &data)
    {
        const char *base_64_characters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

        // handle the empty byte array
        if (data.empty())
        {
            return "";
        }

        // determine the output length and allocate a large enough string
        auto output_length = get_length_of_output_string(data.size());
        std::string output;
        output.resize(output_length);

        size_t output_index = 0;
        for (size_t input_index = 0; input_index < data.size(); input_index += 3)
        {
            // combine three input bytes into a 32-bit unsigned integer, which we can more easily work with
            uint32_t triple = data[input_index];
            if (input_index + 1 < data.size())
            {
                triple = triple << 8 | data[input_index + 1];
            }
            else
            {
                triple = triple << 8;
            }
            if (input_index + 2 < data.size())
            {
                triple = triple << 8 | data[input_index + 2];
            }
            else
            {
                triple = triple << 8;
            }

            // lookup the corresponding characters and set them at the correct place in the output string
            output[output_index] = base_64_characters[(triple >> 18) & 0x3F];
            output[output_index + 1] = base_64_characters[(triple >> 12) & 0x3F];

            // the remaining two characters can either be padding characters (if we consumed all bytes) or they are
            // encoded
            if (input_index + 1 < data.size())
            {
                output[output_index + 2] = base_64_characters[(triple >> 6) & 0x3F];
            }
            else
            {
                // padding
                output[output_index + 2] = '=';
            }
            if (input_index + 2 < data.size())
            {
                output[output_index + 3] = base_64_characters[triple & 0x3F];
            }
            else
            {
                // padding
                output[output_index + 3] = '=';
            }

            // also increase the output index
            output_index += 4;
        }

        return output;
    }

    std::vector<uint8_t> decode_from_base_64(const std::string &data)
    {
        // invalid characters and padding are represented by the value 255
        const uint8_t from_base_64[] = {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
                                        255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 62,  255, 62,  255, 63,
                                        52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  255, 255, 255, 255, 255, 255,
                                        255, 0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   10,  11,  12,  13,  14,
                                        15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  255, 255, 255, 255, 63,
                                        255, 26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,
                                        41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,  255, 255, 255, 255, 255};

        // handle the empty case
        if (data.empty())
        {
            return {};
        }

        std::vector<uint8_t> result;

        // reserve storage for the result to avoid unnecessary allocations
        result.reserve(data.size() * 3 / 4);

        for (size_t i = 0; i < data.size(); i += 4)
        {
            // get the base64 values for the current four characters
            uint8_t character_values[4];

            // the first character is always available due to the loop condition
            character_values[0] = (data[i + 0] <= 'z') ? from_base_64[data[i + 0]] : 255;

            if (i + 1 < data.size())
            {
                // lookup the second character
                character_values[1] = (data[i + 1] <= 'z') ? from_base_64[data[i + 1]] : 255;
            }
            else
            {
                // the second character is missing, assume a padding character that is missing
                character_values[1] = from_base_64['='];
            }

            if (i + 2 < data.size())
            {
                // lookup the third character
                character_values[2] = (data[i + 2] <= 'z') ? from_base_64[data[i + 2]] : 255;
            }
            else
            {
                // the third character is missing, assume a padding character that is missing
                character_values[2] = from_base_64['='];
            }

            if (i + 3 < data.size())
            {
                // lookup the fourth character
                character_values[3] = (data[i + 3] <= 'z') ? from_base_64[data[i + 3]] : 255;
            }
            else
            {
                // the fourth character is missing, assume a padding character that is missing
                character_values[3] = from_base_64['='];
            }

            // compress the character values into the original three bytes
            uint8_t byte_data[3];
            byte_data[0] = ((character_values[0] & 0x3f) << 2) + ((character_values[1] & 0x30) >> 4);
            byte_data[1] = ((character_values[1] & 0x0f) << 4) + ((character_values[2] & 0x3c) >> 2);
            byte_data[2] = ((character_values[2] & 0x03) << 6) + ((character_values[3] & 0x3f) >> 0);

            // add the determined bytes to the result iff they do not consist of padding or other
            // invalid characters (Those characters were represented using the value 255)
            if (character_values[1] != 255)
            {
                result.push_back(byte_data[0]);
            }
            if (character_values[2] != 255)
            {
                result.push_back(byte_data[1]);
            }
            if (character_values[3] != 255)
            {
                result.push_back(byte_data[2]);
            }
        }

        return result;
    }

} // namespace LibFluid::Base64
