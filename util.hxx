#ifndef BLACKJACK_UTIL_HXX
#define BLACKJACK_UTIL_HXX

#include <cctype>
#include <initializer_list>
#include <iomanip>
#include <iostream>
#include <locale>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif

namespace util {
auto TrimWhitespace(std::string s) -> std::string {
    // trim from start
    s.erase(std::begin(s), std::find_if(std::begin(s), std::end(s), [](int c) { return !std::isspace(c); }));
    // trim from end
    s.erase(std::find_if(std::rbegin(s), std::rend(s), [](int c) { return !std::isspace(c); }).base(), std::end(s));

    return s;
}

auto MakeLowercase(std::string s) -> std::string {
    for (auto& c : s) {
        c = static_cast<char>(static_cast<unsigned char>(c));
    }
    return s;
}

// CheckNameValidity checks if the passed string is title cased or if words are PascalCased. Names such as "Michael Jordan" or
// "Dimitri McDonald" are valid, but names such as "alexa stones" or "Gregor MCPhil" aren't.
auto CheckNameValidity(std::string_view name) -> bool {
    static std::regex checker(R"(^(?:[A-Z][a-z]+ ?)+$)");

    return std::regex_match(std::begin(name), std::end(name), checker);
}

// GetCommandString prompts the user to enter a command in the console from the given pairs of long forms and short forms of the
// given commands, matching the input either on equality or on the longest common prefix of the input and the long form.
// The first string in the pair is assumed to be the long form, and the second the short one. It is also assumed that the commands
// are lowercase and have no additional whitespace. The input is lowercased and any additional whitespace is trimmed.
auto GetCommandString(std::initializer_list<std::pair<std::string, std::string>> commands) -> std::string {
    std::string input;
    while (true) {
        getline(std::cin, input, '\n');
        input = MakeLowercase(TrimWhitespace(input));

        std::set<std::size_t> unique_common_prefix_lengths;
        std::string closest_command;
        std::size_t longest_prefix_length{}, possibility_count{};
        for (auto& [long_form, short_form] : commands) {
            if (input == long_form || input == short_form) {
                return long_form;
            }
            std::size_t pref_len{};
            for (; pref_len < std::min(input.size(), long_form.size()) && long_form[pref_len] == input[pref_len]; pref_len++) {}
            if (pref_len == 0) {
                continue;
            }
            possibility_count++;
            unique_common_prefix_lengths.insert(pref_len);
            if (pref_len > longest_prefix_length) {
                longest_prefix_length = pref_len;
                closest_command = long_form;
            }
        }

        if (possibility_count != unique_common_prefix_lengths.size() || possibility_count == 0) {
            std::cout << "Comanda gresita, incearca din nou: ";
            continue;
        }

        std::cout << "Comanda " << std::quoted(closest_command) << " a fost selectata.";

        return closest_command;
    }
}

auto ClearConsole() -> void {
#ifdef _WIN32
    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE) return;

    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi)) return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    if (!FillConsoleOutputCharacter(
        hStdOut,
        (TCHAR) ' ',
        cellCount,
        homeCoords,
        &count
    ))
        return;

    if (!FillConsoleOutputAttribute(
        hStdOut,
        csbi.wAttributes,
        cellCount,
        homeCoords,
        &count
    ))
        return;

    SetConsoleCursorPosition(hStdOut, homeCoords);
#else
    std::cout << "\033[H\033[2J";
#endif
}
} //namespace util

#endif //BLACKJACK_UTIL_HXX
