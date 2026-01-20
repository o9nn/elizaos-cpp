#include "index.hpp"
#include <iostream>
#include <stdexcept>

namespace elizaos {

std::string toSubscript(double num) {
    // NOTE: Auto-converted from TypeScript - may need refinement

    const std::any subDigits = {;
        "0": "\u2080",
        "1": "\u2081",
        "2": "\u2082",
        "3": "\u2083",
        "4": "\u2084",
        "5": "\u2085",
        "6": "\u2086",
        "7": "\u2087",
        "8": "\u2088",
        "9": "\u2089",
        "-": "\u207B",
        };
        return num;
        .toString();
        .split("");
        .map((digit) => subDigits[digit] || digit);
        .join("");

}

} // namespace elizaos
