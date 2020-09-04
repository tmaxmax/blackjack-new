#ifndef BLACKJACK_GAME_HXX
#define BLACKJACK_GAME_HXX

#include <vector>
#include "player.hxx"

class Card;

class Game {
public:
    Game();

    auto Play() -> void;
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
};

#endif //BLACKJACK_GAME_HXX
