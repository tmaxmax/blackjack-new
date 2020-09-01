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

    Card(int value, Pip pip, Suit suit) noexcept;
    Card(const Card&) = default;
    Card(Card&&) = default;
    Card& operator=(const Card& other) = default;
    [[nodiscard]] Suit GetSuit() const noexcept;
    // GetValue returns the card's value.
    [[nodiscard]] int GetValue() const noexcept;
    // SetValue sets the value of an Ace card. It is a no-op for other suits.
    void SetValue(int new_value) noexcept;
    // operator<< writes a string representation of the card to the stream.
    friend std::ostream& operator<<(std::ostream&, Card&);

private:
    int value_;
    Pip pip_;
    Suit suit_;
};

#endif //BLACKJACK_CARD_HXX
