#include "Base45_decoder.hpp"

#include <math.h>

std::vector<uint8_t> base45Decode(const std::string &input)
{
    const std::string BASE45_ALPHABET = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ $%*+-./:";

    std::vector<uint8_t> output;
    size_t length = input.size();
    bool error = false;

    for (size_t i = 0; i < length; i += 3)
    {
        if (error == true)
        {
            output.clear();
            break;
        }
        int x = 0;
        for (size_t j = 0; j < 3 && i + j < length; ++j)
        {
            auto pos = BASE45_ALPHABET.find(input[i + j]);
            if (pos == std::string::npos)
            {
                error = true;
                break;
            }
            x += pos * static_cast<int>(pow(45, j));
        }

        if (i + 2 < length)
        {
            output.push_back(static_cast<uint8_t>(x / 256));
            output.push_back(static_cast<uint8_t>(x % 256));
        }
        else if (i + 1 < length)
        {
            output.push_back(static_cast<uint8_t>(x % 256));
        }
    }

    return output;
}