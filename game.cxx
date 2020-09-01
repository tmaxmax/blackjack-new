#include <algorithm>
#include <iomanip>
#include <iostream>

#include "game.hxx"
#include "player.hxx"
#include "util.hxx"

Game::Game()
    : player_(""), dealer_(), card_stack_(), dev_(std::random_device{}()) {
    std::cout << R"(BLACKJACK 21
Bun venit la unicul joc corect din oras, unde cartile sunt amestecate aleator si nimeni nu te pacaleste!
)" << "Spune-ne numele tau: ";
    std::string name;
    std::getline(std::cin, name, '\n');

    player_ = Player(name);

    std::cout << "Salut, " << player_.GetName() << "! Jocul este pregatit, apasa ENTER cand esti gata!";
    std::cin.get();
    std::cout << '\n';
}

void Game::gameplay() {
    static const std::string command_hit("hit"), command_stand("stand"), command_surrender("surrender");
    std::string command;

    auto show_player_cards = [&]() {
        std::cout << "Cartile tale:\n";
        player_.WriteCurrentCards(std::cout);
        std::cout << "Scor (fara asi): " << player_.GetCurrentScore() << "\n\n";
    };
    auto clear_game = [&]() {
        player_.ClearCards();
        dealer_.ClearCards();
        card_stack_.clear();
    };

    for (int i{}; i < 2; i++) {
        player_.AddCard(getCard());
        dealer_.AddCard(getCard());
    }

    while (true) {
        show_player_cards();

        std::cout << R"("hit", "stand" sau "surrender"? )";

        while (true) {
            getline(std::cin, command, '\n');
            TransformToLowerCase(command);

            if (command == command_hit || command == command_stand || command == command_surrender) {
                break;
            }
            std::cout << "Intentie gresita! incearca din nou.\n";
        }

        if (command != command_hit) {
            break;
        }

        player_.AddCard(getCard());
        std::cout << "Dealerul ia o carte...\n\n";

        auto back_card = card_stack_.back();
        auto dealer_score = dealer_.GetCurrentScore();
        if (dealer_score + back_card.GetValue() < 21 || (back_card.GetSuit() == Card::Suit::Ace && dealer_score + 1 < 21)) {
            dealer_.AddCard(getCard());
        }
    }

    std::cout << '\n';

    if (command == command_surrender) {
        std::cout << "Te-ai dat batut!\n";
        player_.Surrender();
        dealer_.Win();
        if (dealer_.GetCurrentScore() == 21) {
            dealer_.Blackjack();
        }
        clear_game();
        return;
    }

    if (player_.GetAceCount() > 0 && player_.GetCurrentScore() <= 21) {
        show_player_cards();

        int player_ace_value;
        do {
            while (!(std::cin >> player_ace_value)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Valoare invalida, incearca din nou: ";
            }
        } while (player_.SetAce(player_ace_value));
        std::cout << '\n';
    }

    // TODO: optimize dealer algorithm
    auto dealer_ace_test = dealer_;
    auto set_aces = [](Dealer& d) {
        int ace_value;
        do {
            ace_value = 1;
            auto current_score = d.GetCurrentScore();
            if (current_score + 11 <= 21) {
                ace_value = 11;
            }
        } while (d.SetAce(ace_value));
    };
    set_aces(dealer_ace_test);
    for (auto current_score = dealer_.GetCurrentScore();; current_score = dealer_.GetCurrentScore()) {
        if (current_score + dealer_.GetAceCount() + std::max(card_stack_.back().GetValue(), 1) <= 21) {
            dealer_.AddCard(getCard());
        } else {
            break;
        }
    }
    set_aces(dealer_);
    if (dealer_ace_test.GetCurrentScore() > dealer_.GetCurrentScore()) {
        dealer_ = dealer_ace_test;
    }

    auto dealer_score = dealer_.GetCurrentScore();
    auto player_score = player_.GetCurrentScore();

    std::cout << "Cartile tale:\n";
    player_.WriteCurrentCards(std::cout);
    std::cout << "Scor: " << player_score << '\n';
    std::cout << "Cartile dealerului:\n";
    dealer_.WriteCurrentCards(std::cout);
    std::cout << "Scor: " << dealer_score << "\n\n";

    if (player_score == dealer_score || (player_score > 21 && dealer_score > 21)) {
        std::cout << "Egalitate!\n\n\n";
        return;
    }
    if (player_score > 21) {
        std::cout << "Bust, " << player_.GetName() << "!\n";
        player_.Lose();
        player_.Bust();
        dealer_.Win();
    }
    if (dealer_score > 21) {
        std::cout << "Bust pentru dealer!\n";
        dealer_.Lose();
        dealer_.Bust();
        player_.Win();
    }
    if (player_score == 21) {
        std::cout << "Blackjack, " << player_.GetName() << "!\n";
        player_.Blackjack();
    }
    if (dealer_score == 21) {
        std::cout << "Blackjack pentru dealer!\n";
        dealer_.Blackjack();
    }
    if (player_score > dealer_score && player_score <= 21) {
        std::cout << "Ai castigat, " << player_.GetName() << "!\n";
        player_.Win();
        dealer_.Lose();
    }
    if (dealer_score > player_score && dealer_score <= 21) {
        std::cout << "Dealerul a castigat!\n";
        dealer_.Win();
        player_.Lose();
    }
    std::cout << "\n\n";

    clear_game();
}

Card Game::getCard() noexcept {
    makeStack();
    auto ret = card_stack_.back();
    card_stack_.pop_back();
    return ret;
}

void Game::makeStack() {
    if (!card_stack_.empty()) {
        return;
    }

    static Card::Pip pips[] = {Card::Pip::Clubs, Card::Pip::Diamonds, Card::Pip::Hearts, Card::Pip::Spades};
    static Card::Suit suits[] = {Card::Suit::Jack, Card::Suit::Queen, Card::Suit::King, Card::Suit::Ace, Card::Suit::Numeral};

    for (auto s : suits) {
        for (auto p : pips) {
            switch (s) {
            case Card::Suit::Numeral:
                for (int i = 1; i < 11; i++) {
                    card_stack_.emplace_back(i, p, s);
                }
                break;
            case Card::Suit::Ace:
                card_stack_.emplace_back(0, p, s);
                break;
            case Card::Suit::King:
            case Card::Suit::Queen:
            case Card::Suit::Jack:
                card_stack_.emplace_back(10, p, s);
                break;
            }
        }
    }

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
    stats();
}

void Game::stats() {
    auto write_player_stats = [](BasePlayer& b) {
        std::cout << std::setw(12) << b.GetWinCount()
                  << std::setw(12) << b.GetLossCount()
                  << std::setw(12) << b.GetBlackjackCount()
                  << std::setw(12) << b.GetBustCount()
                  << std::setw(12) << b.GetSurrenderCount();
    };
    std::cout << "      ";
    std::cout << std::setw(12) << "Wins"
              << std::setw(12) << "Losses"
              << std::setw(12) << "Blackjacks"
              << std::setw(12) << "Busts"
              << std::setw(12) << "Surrenders";
    std::cout << "\nPlayer";
    write_player_stats(player_);
    std::cout << "\nDealer";
    write_player_stats(dealer_);
}
