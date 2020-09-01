#include "card.hxx"

#include <string_view>
#include <utility>

Card::Card(int value, Pip pip, Suit suit) noexcept
    : value_(value), pip_(pip), suit_(suit) {}

int Card::GetValue() const noexcept {
    return value_;
}

void Card::SetValue(int new_value) noexcept {
    if (suit_ != Suit::Ace || (new_value != 1 && new_value != 11)) {
        return;
    }
    value_ = new_value;
}

std::ostream& operator<<(std::ostream& os, Card& c) {
    switch (c.suit_) {
    case Card::Suit::Numeral:
        os << c.GetValue();
        break;
    case Card::Suit::Ace:
        os << "As";
        if (c.GetValue() != 0) {
            os << " (" << c.GetValue() << ')';
        }
        break;
    case Card::Suit::King:
        os << "Rege";
        break;
    case Card::Suit::Queen:
        os << "Regina";
        break;
    case Card::Suit::Jack:
        os << "Valet";
        break;
    }

    os << " de ";

    switch (c.pip_) {
    case Card::Pip::Spades:
        os << "pica";
        break;
    case Card::Pip::Hearts:
        os << "inima rosie";
        break;
    case Card::Pip::Diamonds:
        os << "romb";
        break;
    case Card::Pip::Clubs:
        os << "trefla";
        break;
    }
    return os;
}

Card::Suit Card::GetSuit() const noexcept {
    return suit_;
}
