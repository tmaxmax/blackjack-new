#include <algorithm>
#include <iomanip>
#include <iostream>
#include <string_view>

#include "game.hxx"
#include "card.hxx"
#include "player.hxx"
#include "util.hxx"

Game::Game()
    : player_(""), computer_(""), card_stack_(), dev_(std::random_device{}()) {
    std::cout << R"(BLACKJACK 21
Bun venit la unicul joc corect din oras, unde cartile sunt amestecate aleator si nimeni nu te pacaleste!
)" << "Spune-ne numele tau: ";

    // get the player's name
    std::string name;
    std::getline(std::cin, name, '\n');

    player_ = Player(name);

    // greet the player
    std::cout << "Salut, " << player_.GetName() << "! Jocul este pregatit, apasa ENTER cand esti gata!";
    std::cin.get();
    std::cout << '\n';
}

void Game::gameplay() {
    // initialize command constants
    static const std::string command_hit("hit"), command_stand("stand"), command_surrender("surrender");
    std::string command;

    auto show_cards = [](const Player& p) {
        if (p.GetName().empty()) {
            std::cout << "Cartile computerului:\n";
        } else {
            std::cout << "Cartile tale:\n";
        }
        p.WriteCurrentCards(std::cout);
        std::cout << "Scor: " << p.GetCurrentScore() << "\n";
    };

    auto get_maximum_score = [](const Player& p, Card new_card = Card(0, Card::Suit::Numeral, Card::Pip::Clubs)) -> int {
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

    while (true) {
        show_cards(player_);

        std::cout << std::quoted(command_hit) << ", "
                  << std::quoted(command_stand) << " sau "
                  << std::quoted(command_surrender) << "? ";

        // get the user's command
        while (true) {
            getline(std::cin, command, '\n');
            TransformToLowerCase(command);

            // break on valid command
            if (command == command_hit || command == command_stand || command == command_surrender) {
                break;
            }
            // if the command is invalid, prompt the user to input it again.
            std::cout << "Intentie gresita! incearca din nou: \n";
        }

        // if the user doesn't hit, stop drawing cards
        if (command != command_hit) {
            break;
        }

        // give the player a card
        player_.AddCard(getCard());

        // if the computer doesn't draw cards anymore, continue.
        if (!computer_must_draw) {
            std::cout << '\n';
            continue;
        }
        // else give the computer a card
        std::cout << "Computerul ia o carte...\n\n";

        auto maximum_score_no_card = get_maximum_score(computer_);
        auto maximum_score_new_card = get_maximum_score(computer_, card_stack_.back());
        if (maximum_score_new_card > maximum_score_no_card && maximum_score_new_card <= 21) {
            computer_.AddCard(getCard());
        } else {
            computer_must_draw = false;
        }
    }

    std::cout << '\n';

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
    show_cards(player_);
    for (std::size_t i{}; i < player_.GetAceCount(); i++) {
        int player_ace_value;
        std::cout << "Introdu valoarea asului " << i + 1 << " (1 sau 11): ";
        // read values from user until it inputs eiter 1 or 11.
        while (!(std::cin >> player_ace_value) || !player_.SetAce(i, player_ace_value)) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Valoare invalida, incearca din nou (1 sau 11): ";
        }
        // ignore the last newline
        std::cin.ignore();
    }

    auto set_aces = [](Player& p) {
        for (std::size_t i{}; i < p.GetAceCount(); i++) {
            auto ace_value = 1;
            if (p.GetCurrentScore() + 11 <= 21) {
                ace_value = 11;
            }
            p.SetAce(i, ace_value);
        }
    };

    while (true) {
        auto computer_temp = computer_;

        if (!computer_must_draw) {
            break;
        }

        auto maximum_score_no_card = get_maximum_score(computer_temp);
        auto maximum_score_new_card = get_maximum_score(computer_temp, card_stack_.back());
        if (maximum_score_new_card <= 21 && maximum_score_new_card > maximum_score_no_card) {
            computer_temp.AddCard(getCard());
            computer_ = computer_temp;
        } else {
            break;
        }
    }

    set_aces(computer_);

    auto dealer_score = computer_.GetCurrentScore();
    auto player_score = player_.GetCurrentScore();

    show_cards(computer_);

    std::cout << '\n';

    if (player_score == dealer_score || (player_score > 21 && dealer_score > 21)) {
        std::cout << "Egalitate!\n\n\n";
        return;
    }
    if (player_score > 21) {
        std::cout << "Bust, " << player_.GetName() << "!\n";
        player_.Lose();
        player_.Bust();
        computer_.Win();
    }
    if (dealer_score > 21) {
        std::cout << "Bust pentru dealer!\n";
        computer_.Lose();
        computer_.Bust();
        player_.Win();
    }
    if (player_score == 21) {
        std::cout << "Blackjack, " << player_.GetName() << "!\n";
        player_.Blackjack();
    }
    if (dealer_score == 21) {
        std::cout << "Blackjack pentru dealer!\n";
        computer_.Blackjack();
    }
    if (player_score > dealer_score && player_score <= 21) {
        std::cout << "Ai castigat, " << player_.GetName() << "!\n";
        player_.Win();
        computer_.Lose();
    }
    if (dealer_score > player_score && dealer_score <= 21) {
        std::cout << "Dealerul a castigat!\n";
        computer_.Win();
        player_.Lose();
    }
    std::cout << "\n\n";
}

Card Game::getCard() noexcept {
    makeStack();
    auto ret = card_stack_.back();
    card_stack_.pop_back();
    return ret;
}

void Game::makeStack() {
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

    // shuffle the cards
    std::shuffle(std::begin(card_stack_), std::end(card_stack_), dev_);
}

void Game::Play() {
    while (true) {
        std::cout << R"(MENIU:
 - ("play") Joaca!
 - ("exit") Inchide jocul.
Introdu o optiune: )";
        std::string command;
        while (true) {
            getline(std::cin, command, '\n');
            TransformToLowerCase(command);
            if (command == "exit" || command == "play") {
                break;
            }
            std::cout << "Intentie invalida, incearca din nou: ";
        }
        if (command == "exit") {
            break;
        }
        std::cout << '\n';
        gameplay();
    }
    std::cout << '\n';
    showStats();
}

void Game::showStats() {
    auto write = [](std::string_view text = "") {
        std::cout << '\n' << std::setfill(' ') << std::setw(8) << text;
    };
    auto write_player_stats = [](Player& b) {
        std::cout << std::setw(12) << b.GetWinCount()
                  << std::setw(12) << b.GetLossCount()
                  << std::setw(12) << b.GetBlackjackCount()
                  << std::setw(12) << b.GetBustCount()
                  << std::setw(12) << b.GetSurrenderCount();
    };
    std::cout << "STATISTICI";
    write();
    std::cout << std::setw(12) << "Castiguri"
              << std::setw(12) << "Pierderi"
              << std::setw(12) << "Blackjacks"
              << std::setw(12) << "Busts"
              << std::setw(12) << "Surrenders";
    write("Tu");
    write_player_stats(player_);
    write("Computer");
    write_player_stats(computer_);
}
