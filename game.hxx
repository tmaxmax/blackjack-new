#ifndef BLACKJACK_GAME_HXX
#define BLACKJACK_GAME_HXX

#include <memory>
#include <random>
#include <vector>
#include "player.hxx"

class Card;

class Game {
public:
    Game();

    void Play();
private:
    void gameplay();
    void stats();
    // getCards refills the stack if no cards are available and returns the last card in the stack;
    [[nodiscard]] Card getCard() noexcept;
    // makeStack generates a new shuffled stack of cards.
    void makeStack();

    Player player_;
    Dealer dealer_;
    std::vector<Card> card_stack_;
    std::mt19937 dev_;
};

#endif //BLACKJACK_GAME_HXX
