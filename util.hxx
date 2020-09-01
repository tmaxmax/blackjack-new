#ifndef BLACKJACK_UTIL_HXX
#define BLACKJACK_UTIL_HXX

#include <string>

void TransformToLowerCase(std::string& s) {
    for (auto& c : s) {
        c = static_cast<char>(tolower(c));
    }
}

#endif //BLACKJACK_UTIL_HXX
