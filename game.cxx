#include <algorithm>
#include <iomanip>
#include <iostream>
#include <random>
#include <string_view>

#include "game.hxx"
#include "card.hxx"
#include "player.hxx"
#include "util.hxx"

// TODO: Add cross-platform console clear
// TODO: Add colored output

Game::Game()
    : player_(""), computer_(""), card_stack_(), game_count_(), draw_count_() {
    util::ClearConsole();
    std::cout << R"(BLACKJACK 21
Bun venit la unicul joc corect din oras, unde cartile sunt amestecate aleator si nimeni nu te pacaleste!
Spune-ne numele tau: )";

    // get the player's name
    std::string name;
    while (true) {
        std::getline(std::cin, name, '\n');
        name = util::TrimWhitespace(name);
        if (name.empty()) {
            std::cout << "Numele tau nu poate fi gol! Incearca din nou: ";
            continue;
        }
        if (!util::CheckNameValidity(name)) {
            std::cout << R"(Esti sigur ca ti-ai introdus bine numele ("da" sau "nu")? )";
            if (util::GetCommandString({"da", "nu"}) == "nu") {
                std::cout << "Introdu-ti din nou numele: ";
                continue;
            }
        }
        // if this point is reached, the name is valid
        break;
    }

    player_ = Player(name);

    // greet the player
    std::cout << "Salut, " << player_.GetName() << "! Jocul este pregatit, apasa ENTER cand esti gata!";
    std::cin.get();
    util::ClearConsole();
}

auto Game::gameplay() -> void {
    // initialize command constants
    static const std::string command_hit("hit"), command_stand("stand"), command_surrender("surrender");

    auto get_maximum_score = [](const Player& p, Card new_card = Card::empty) {
        auto ace_count = p.GetAceCount();
        if (new_card.GetSuit() == Card::Suit::Ace) {
            ace_count++;
        }
        auto score = p.GetCurrentScore() + static_cast<int>(ace_count) * 11 + new_card.GetValue();

        while (score > 21 && ace_count > 0) {
            score -= 10;
            ace_count--;
        }

        return score;
    };
    auto get_minimum_score = [](const Player& p, Card new_card = Card::empty) {
        auto ace_count = p.GetAceCount();
        if (new_card.GetSuit() == Card::Suit::Ace) {
            ace_count++;
        }
        return p.GetCurrentScore() + static_cast<int>(ace_count) * 1 + new_card.GetValue();
    };
    auto should_draw = [&get_minimum_score, &get_maximum_score](const Player& p, Card new_card) {
        const auto min_score = get_minimum_score(p);
        const auto min_score_new_card = get_minimum_score(p, new_card);
        const auto max_score = get_maximum_score(p);
        const auto max_score_new_card = get_maximum_score(p, new_card);
        return (max_score_new_card <= 21 && max_score_new_card > max_score) ||
               (min_score_new_card <= 21 && min_score_new_card > min_score);
    };

    auto show_cards = [&get_maximum_score](const Player& p, const bool show_maximum = false) {
        if (p.GetName().empty()) {
            std::cout << "Cartile computerului:\n";
        } else {
            std::cout << "Cartile tale:\n";
        }
        p.WriteCurrentCards(std::cout);
        const auto current_score = p.GetCurrentScore();
        std::cout << "Scor: " << current_score;
        if (show_maximum) {
            const auto maximum_score = get_maximum_score(p);
            if (maximum_score != current_score) {
                std::cout << " (maxim " << maximum_score << " cu tot cu asi)";
            }
        }
        std::cout << '\n';
    };

    // initialize the states of the players and the card stack
    player_.Init();
    computer_.Init();
    card_stack_.clear();

    // add 2 cards for each player, as stated by the rules
    for (int i{}; i < 2; i++) {
        player_.AddCard(getCard());
        computer_.AddCard(getCard());
    }
    auto computer_must_draw{true};

    std::string command;
    while (true) {
        util::ClearConsole();

        show_cards(player_, true);

        std::cout << std::quoted(command_hit) << ", "
                  << std::quoted(command_stand) << " sau "
                  << std::quoted(command_surrender) << "? ";

        // get the user's command
        command = util::GetCommandString({command_hit, command_stand, command_surrender});

        // if the user doesn't hit, stop drawing cards
        if (command != command_hit) {
            break;
        }

        // give the player a card
        player_.AddCard(getCard());

        std::cout << '\n';
        // if the computer doesn't draw cards anymore, continue.
        if (!computer_must_draw) {
            continue;
        }

        if (should_draw(computer_, card_stack_.back())) {
            computer_.AddCard(getCard());
        } else {
            computer_must_draw = false;
        }
    }

   util::ClearConsole();

    // if he player surrendered, end the game earlier.
    if (command == command_surrender) {
        std::cout << "Te-ai dat batut!\n";
        player_.Surrender();
        computer_.Win();
        // give a blackjack to the computer if it has 21 points.
        if (computer_.GetCurrentScore() == 21) {
            computer_.Blackjack();
        }
        return;
    }

    // set the player's aces
    show_cards(player_, true);
    for (std::size_t i{}; i < player_.GetAceCount(); i++) {
        int player_ace_value;
        std::cout << "Introdu valoarea asului " << i + 1 << " (1 sau 11): ";
        // read values from user until it inputs eiter 1 or 11.
        while (!(std::cin >> player_ace_value) || !player_.SetAce(i, player_ace_value)) {
            std::cin.clear();
            std::cin.ignore(std::cin.rdbuf()->in_avail());
            std::cout << "Valoare invalida, incearca din nou (1 sau 11): ";
        }
        // ignore the last newline
        std::cin.ignore();
    }

    auto set_aces = [](Player& p) {
        std::size_t i{};
        for (; i < p.GetAceCount(); i++) {
            auto ace_value = 1;
            if (p.GetCurrentScore() + 11 <= 21) {
                ace_value = 11;
            }
            p.SetAce(i, ace_value);
        }
        while (p.GetCurrentScore() > 21) {
            p.SetAce(--i, 1);
        }
    };

    while (computer_must_draw) {
        auto computer_temp = computer_;

        if (should_draw(computer_temp, card_stack_.back())) {
            computer_temp.AddCard(getCard());
            computer_ = computer_temp;
        } else {
            computer_must_draw = false;
        }
    }

    set_aces(computer_);

    auto computer_score = computer_.GetCurrentScore();
    auto player_score = player_.GetCurrentScore();

    util::ClearConsole();
    show_cards(player_);
    std::cout << '\n';
    show_cards(computer_);

    std::cout << '\n';

    if (player_score == computer_score || (player_score > 21 && computer_score > 21)) {
        std::cout << "Egalitate!\n\n\n";
        draw_count_++;
        return;
    }
    if (player_score > 21) {
        std::cout << "Bust, " << player_.GetName() << "!\n";
        player_.Lose();
        player_.Bust();
        computer_.Win();
    }
    if (computer_score > 21) {
        std::cout << "Bust pentru computer!\n";
        computer_.Lose();
        computer_.Bust();
        player_.Win();
    }
    if (player_score == 21) {
        std::cout << "Blackjack, " << player_.GetName() << "!\n";
        player_.Blackjack();
    }
    if (computer_score == 21) {
        std::cout << "Blackjack pentru computer!\n";
        computer_.Blackjack();
    }
    if (player_score > computer_score && player_score <= 21) {
        std::cout << "Ai castigat, " << player_.GetName() << "!\n";
        player_.Win();
        computer_.Lose();
    }
    if (computer_score > player_score && computer_score <= 21) {
        std::cout << "Computerul a castigat!\n";
        computer_.Win();
        player_.Lose();
    }
}

auto Game::getCard() noexcept -> Card {
    makeStack();
    auto ret = card_stack_.back();
    card_stack_.pop_back();
    return ret;
}

auto Game::makeStack() -> void {
    // if there are cards in the stack return
    if (!card_stack_.empty()) {
        return;
    }

    static Card::Pip pips[] = {Card::Pip::Clubs, Card::Pip::Diamonds, Card::Pip::Hearts, Card::Pip::Spades};
    static Card::Suit suits[] = {Card::Suit::Jack, Card::Suit::Queen, Card::Suit::King, Card::Suit::Ace, Card::Suit::Numeral};

    // add a card to the stack for each pip and suit
    for (auto s : suits) {
        for (auto p : pips) {
            switch (s) {
                // add every numeral card
            case Card::Suit::Numeral:
                for (int i = 1; i < 11; i++) {
                    card_stack_.emplace_back(i, s, p);
                }
                break;
            case Card::Suit::Ace:
                card_stack_.emplace_back(0, s, p);
                break;
            default:
                card_stack_.emplace_back(10, s, p);
                break;
            }
        }
    }

    static std::mt19937 dev(std::random_device{}());
    // shuffle the cards
    std::shuffle(std::begin(card_stack_), std::end(card_stack_), dev);
}

auto Game::Play() -> void {
    while (true) {
        std::cout << R"(MENIU:
 - ("play") Joaca!
 - ("stats") Vezi statistici.
 - ("exit") Inchide jocul.
Introdu o optiune: )";
        auto command = util::GetCommandString({"play", "stats", "exit"});
        std::cout << '\n';
        if (command == "play") {
            gameplay();
            game_count_++;
        } else if (command == "stats") {
            showStats();
        } else {
            break;
        }
        std::cout << "\n\n";
    }
    std::cout << '\n';
    showStats();
}

auto Game::showStats() -> void {
    util::ClearConsole();
    if (game_count_ == 0) {
        std::cout << "Nu s-a jucat niciun joc!";
        return;
    }

    auto write = [](std::string_view text) {
        std::cout << '\n' << std::setfill(' ') << std::setw(8) << text;
    };
    auto write_player_stats = [](Player& b) {
        std::cout << std::setw(12) << b.GetWinCount()
                  << std::setw(12) << b.GetLossCount()
                  << std::setw(12) << b.GetBlackjackCount()
                  << std::setw(12) << b.GetBustCount()
                  << std::setw(12) << b.GetSurrenderCount();
    };
    std::cout << "STATISTICI (" << game_count_ << " jocuri jucate, " << draw_count_ << " egalitati)";
    write("");
    std::cout << std::setw(12) << "Castiguri"
              << std::setw(12) << "Pierderi"
              << std::setw(12) << "Blackjacks"
              << std::setw(12) << "Busts"
              << std::setw(12) << "Surrenders";
    write("Tu");
    write_player_stats(player_);
    write("Computer");
    write_player_stats(computer_);
    std::cout << '\n';
}
