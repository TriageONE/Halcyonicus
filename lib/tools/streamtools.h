//
// Created by Triage on 4/30/2023.
//

#ifndef HALCYONICUS_STREAMTOOLS_H
#define HALCYONICUS_STREAMTOOLS_H

#include "../../halcyonicus.h"

std::string stringToHex(const std::string& input) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    int line_count = 0;
    for (size_t i = 0; i < input.length(); ++i) {
        if (i != 0 && i % 8 == 0) {
            ss << "\t";
            ++line_count;
        }
        if (i != 0 && i % 16 == 0) {
            ss << std::endl;
            line_count = 0;
        }
        ss << std::setw(2) << static_cast<int>(input[i]);
        if ((i+1) % 8 == 4) {
            ss << " ";
        }
        if (i == input.length() - 1 && i % 8 != 7) {
            ss << std::setw((8 - (i % 8) - 1) * 2) << "";
        }
    }
    return ss.str();
}

#endif //HALCYONICUS_STREAMTOOLS_H
