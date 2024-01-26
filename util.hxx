#ifndef BLACKJACK_UTIL_HXX
#define BLACKJACK_UTIL_HXX

#include <algorithm>
#include <cctype>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>

#ifdef _WIN32
#include <windows.h>
#endif

namespace util {
inline auto TrimWhitespace(std::string s) -> std::string {
    // trim from start
    s.erase(std::begin(s), std::find_if(std::begin(s), std::end(s), [](int c) { return !std::isspace(c); }));
    // trim from end
    s.erase(std::find_if(std::rbegin(s), std::rend(s), [](int c) { return !std::isspace(c); }).base(), std::end(s));

    return s;
}

inline auto MakeLowercase(std::string s) -> std::string {
    std::transform(std::begin(s), std::end(s), std::begin(s), [](unsigned char c) -> char { return std::tolower(c); });
    return s;
}

struct GetCommandStringInput {
    GetCommandStringInput(std::string_view lf, std::string_view sf, std::optional<bool> ae)
        : long_form{lf}
        , short_form{sf}
        , allow_empty{ae} {}
    GetCommandStringInput(std::string_view lf, std::string_view sf)
        : GetCommandStringInput(lf, sf, {}) {}

    std::string_view long_form;
    std::string_view short_form;
    std::optional<bool> allow_empty;
};

// GetCommandString prompts the user to enter a command in the console from the given pairs of long forms and short forms of the
// given commands, matching the input either on long form, short form or long form prefix of input length equality.
// The first string in the pair is assumed to be the long form, and the second the short one. It is also assumed that the commands
// are lowercase and have no additional whitespace. The input is lowercased and any additional whitespace is trimmed.
inline auto GetCommandString(std::initializer_list<GetCommandStringInput> commands) -> std::string_view {
    std::string input;
    while (true) {
        getline(std::cin, input, '\n');
        input = MakeLowercase(TrimWhitespace(input));

        for (auto& [long_form, short_form, allow_empty] : commands) {
            if (input == short_form || input == long_form) {
                return long_form;
            }
            if ((input == long_form.substr(0, input.size()) && std::size(input) > 0) || (input.empty() && allow_empty && *allow_empty)) {
                std::cout << "Comanda \"" << long_form << "\" a fost selectata!\n";
                return long_form;
            }
        }
        std::cout << "Comanda gresita, incearca din nou: ";
    }
}

inline auto ClearConsole() -> void {
#ifdef _WIN32
    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
        return;

    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
        return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(
            hStdOut,
            (TCHAR)' ',
            cellCount,
            homeCoords,
            &count))
        return;

    if (!FillConsoleOutputAttribute(
            hStdOut,
            csbi.wAttributes,
            cellCount,
            homeCoords,
            &count))
        return;

    SetConsoleCursorPosition(hStdOut, homeCoords);
#else
    std::cout << "\033[H\033[2J";
#endif
}
} // namespace util

#endif // BLACKJACK_UTIL_HXX
