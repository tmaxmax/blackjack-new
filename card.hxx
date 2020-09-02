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
    bool SetValue(int new_value) noexcept;
    [[nodiscard]] Suit GetSuit() const noexcept;
    // GetValue returns the card's value.
    [[nodiscard]] int GetValue() const noexcept;
    // operator<< writes a string representation of the card to the stream.
    friend std::ostream& operator<<(std::ostream&, Card);

private:
    int value_;
    Suit suit_;
    Pip pip_;
};

#endif //BLACKJACK_CARD_HXX
