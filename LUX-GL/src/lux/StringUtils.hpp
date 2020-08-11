#pragma once

#include <string>
#include <algorithm>


namespace lux {

    class StringUtils
    {
    public:
        static std::string ToUppercase(std::string text)
        {
            std::transform(text.begin(), text.end(), text.begin(),
                [](unsigned char c) { return std::toupper(c); });
            return text;
        }
    };
}