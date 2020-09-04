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
auto TrimWhitespace(std::string_view s) -> std::string {
    std::string ret(s);
    // trim from start
    ret.erase(std::begin(ret), std::find_if(std::begin(ret), std::end(ret), [](int c) { return !std::isspace(c); }));
    // trim from end
    ret.erase(std::find_if(std::rbegin(ret), std::rend(ret), [](int c) { return !std::isspace(c); }).base(), std::end(ret));

    return ret;
}

auto MakeLowercase(std::string_view s) -> std::string {
    std::string ret;
    ret.reserve(s.size());
    for (unsigned char c : s) {
        ret += std::tolower(c);
    }
    return ret;
}

// CheckNameValidity checks if the passed string is title cased or if words are PascalCased. Names such as "Michael Jordan" or
// "Dimitri McDonald" are valid, but names such as "alexa stones" or "Gregor MCPhil" aren't.
auto CheckNameValidity(std::string_view name) -> bool {
    static std::regex checker(R"(^(?:[A-Z][a-z]+ ?)+$)");

    return std::regex_match(std::begin(name), std::end(name), checker);
}

// GetCommandString prompts the user to enter a command from the given list to console, repeating the same operation until a valid
// command is inputted. The function performs a case-insensitive prefix matching between the user input and the given commands and
// it returns the selected command.
auto GetCommandString(std::initializer_list<std::string_view> commands) -> std::string {
    std::string input;
    while (true) {
        getline(std::cin, input, '\n');
        input = MakeLowercase(TrimWhitespace(input));

        std::unordered_map<std::string, unsigned> common_prefix_length;
        std::set<unsigned> unique_common_prefix_lengths;
        std::string closest_command;
        unsigned longest_prefix_length{};
        for (auto com : commands) {
            auto command = MakeLowercase(TrimWhitespace(com));
            std::size_t i{};
            for (; i < std::min(std::size(input), std::size(command)) && command[i] == input[i]; i++) {}
            if (i == 0) {
                continue;
            }
            common_prefix_length[command] = i;
            unique_common_prefix_lengths.insert(common_prefix_length[command]);
            if (common_prefix_length[command] > longest_prefix_length) {
                longest_prefix_length = common_prefix_length[command];
                closest_command = command;
            }
        }

        if (common_prefix_length.size() != unique_common_prefix_lengths.size()) {
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
