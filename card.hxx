#ifndef BLACKJACK_CARD_HXX
#define BLACKJACK_CARD_HXX

#include <ostream>

class Card {
public:
    enum class Pip {
        Spades, Hearts, Diamonds, Clubs
    };
    enum class Suit {
        Numeral, Ace, King, Queen, Jack
    };

    Card(int value, Suit suit, Pip pip) noexcept;
    // SetValue sets the value of an Ace card (either 1 or 11). It returns true if the value was set.
    auto SetValue(int new_value) noexcept -> bool;
    [[nodiscard]] auto GetSuit() const noexcept -> Suit;
    // GetValue returns the card's value.
    [[nodiscard]] auto GetValue() const noexcept -> int;
    // operator<< writes a string representation of the card to the stream.
    friend auto operator<<(std::ostream&, Card) -> std::ostream&;

    static const Card empty;
private:
    int value_;
    Suit suit_;
    Pip pip_;
};

#endif //BLACKJACK_CARD_HXX
