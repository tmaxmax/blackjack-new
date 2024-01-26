#ifndef BLACKJACK_GAME_HXX
#define BLACKJACK_GAME_HXX

#include "player.hxx"

#include <random>
#include <vector>

class Game {
public:
    Game();

    auto Play() -> void;
    auto RegisterPlayer() -> bool;

private:
    auto gameplay() -> void;
    auto showStats() -> void;
    // makeStack generates a new shuffled stack of cards.
    auto makeStack() -> void;
    // getCards refills the stack if no cards are available and returns the last card in the stack;
    [[nodiscard]] auto getCard() noexcept -> Card;

    Player player_;
    Player computer_;
    std::vector<Card> card_stack_;
    int game_count_;
    int draw_count_;
    std::mt19937 dev_;
};

#endif // BLACKJACK_GAME_HXX
