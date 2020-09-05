#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string_view>

#include "game.hxx"
#include "card.hxx"
#include "player.hxx"
#include "util.hxx"

// TODO: Add cross-platform console clear
// TODO: Add colored output

Game::Game()
    : player_(""), computer_(""), card_stack_(), game_count_(), draw_count_(), dev_(std::random_device{}()) {
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
        // if this point is reached, the name is valid
        break;
    }

    player_ = Player(name);

    // greet the player
    std::cout << "Salut, " << player_.GetName() << "! Jocul este pregatit.\n";
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

    enum class Action {
        Hit, Stand, Surrender
    };
    auto draw_card = [&](Player& p) -> Action {
        // add a bust possibility every ~10 games
        static std::bernoulli_distribution dist(0.1);
        const auto min_score = get_minimum_score(p);
        const auto min_score_new_card = get_minimum_score(p, card_stack_.back());
        const auto max_score = get_maximum_score(p);
        const auto max_score_new_card = get_maximum_score(p, card_stack_.back());
        const auto should_draw = (max_score_new_card <= 21 && max_score_new_card > max_score) ||
                                 (min_score_new_card <= 21 && min_score_new_card > min_score) ||
                                 (dist(dev_) && max_score != 21 && min_score != 21);
        if (should_draw) {
            p.AddCard(getCard());
            if (p.GetCurrentScore() > 21 && dist(dev_)) {
                return Action::Surrender;
            }
            return Action::Hit;
        }
        return Action::Stand;
    };

    auto show_cards = [&get_maximum_score](const Player& p, const bool show_maximum) {
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
    auto last_computer_action{Action::Hit};

    std::string command;
    while (true) {
        util::ClearConsole();

        show_cards(player_, true);

        std::cout << R"("hit" (h), "stand" (s) sau "surrender" (surr)? )";

        // get the user's command
        command = util::GetCommandString({{command_hit,       "h"},
                                          {command_stand,     "s"},
                                          {command_surrender, "surr"}});

        // if the user doesn't hit, stop drawing cards
        if (command != command_hit) {
            break;
        }

        // give the player a card
        player_.AddCard(getCard());

        std::cout << '\n';
        // if the computer doesn't draw cards anymore, continue.
        if (last_computer_action != Action::Hit) {
            continue;
        }

        last_computer_action = draw_card(computer_);
    }

    util::ClearConsole();

    // if he player surrendered, end the game earlier.
    if (command == command_surrender) {
        std::cout << "Te-ai dat batut!\n";
        player_.Surrender();
        player_.Lose();
        computer_.Win();

        if (computer_.GetCurrentScore() == 21) {
            // give a blackjack to the computer if it has 21 points.
            computer_.Blackjack();
        }
        return;
    }

    auto set_aces = [](Player& p) {
        for (std::size_t i{}; i < p.GetAceCount(); i++) {
            auto ace_value = 1;
            if (p.GetCurrentScore() + 11 <= 21) {
                ace_value = 11;
            }
            p.SetAce(i, ace_value);
        }
        for (std::size_t i{}; i < p.GetAceCount() && p.GetCurrentScore() > 21; i++) {
            p.SetAce(i, 1);
        }
    };

    while (true) {
        auto computer_temp = computer_;
        last_computer_action = draw_card(computer_temp);
        if (last_computer_action == Action::Hit) {
            computer_ = computer_temp;
        } else {
            break;
        }
    }

    set_aces(player_);
    set_aces(computer_);

    auto computer_score = computer_.GetCurrentScore();
    auto player_score = player_.GetCurrentScore();

    util::ClearConsole();
    show_cards(player_, false);
    std::cout << '\n';
    show_cards(computer_, false);

    std::cout << '\n';

    if (player_score == computer_score) {
        std::cout << "Egalitate!\n";
        draw_count_++;
    }
    if (player_score > 21) {
        std::cout << "Bust, " << player_.GetName() << "!\n";
        player_.Lose();
        player_.Bust();
    }
    if (computer_score > 21) {
        std::cout << "Bust pentru computer!\n";
        computer_.Lose();
        computer_.Bust();
    }
    if (player_score == 21) {
        std::cout << "Blackjack, " << player_.GetName() << "!\n";
        player_.Blackjack();
    }
    if (computer_score == 21) {
        std::cout << "Blackjack pentru computer!\n";
        computer_.Blackjack();
    }
    if ((player_score > computer_score && player_score <= 21) || computer_score > 21) {
        std::cout << "Ai castigat, " << player_.GetName() << "!\n";
        player_.Win();
        if (computer_score < 21) {
            computer_.Lose();
        }
    }
    if ((computer_score > player_score && computer_score <= 21) || player_score > 21) {
        std::cout << "Computerul a castigat!\n";
        computer_.Win();
        if (player_score < 21) {
            player_.Lose();
        }
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
                for (int i = 2; i < 11; i++) {
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

    // shuffle the cards
    std::shuffle(std::begin(card_stack_), std::end(card_stack_), dev_);
}

auto Game::Play() -> void {
    while (true) {
        std::cout << R"(

MENIU:
 - (p | "play") Joaca!
 - (s | "stats") Vezi statistici.
 - (e | "exit") Inchide jocul.
Introdu o optiune: )";
        auto command = util::GetCommandString({{"play",  "p"},
                                               {"stats", "s"},
                                               {"exit",  "e"}});
        std::cout << '\n';
        if (command == "play") {
            gameplay();
            game_count_++;
        } else if (command == "stats") {
            showStats();
        } else {
            break;
        }
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
