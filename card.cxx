#include "card.hxx"

Card::Card(int value, Suit suit, Pip pip) noexcept
    : value_(value), suit_(suit), pip_(pip) {}

auto Card::SetValue(int new_value) noexcept -> bool {
    if (suit_ != Suit::Ace || (new_value != 1 && new_value != 11)) {
        return false;
    }
    value_ = new_value;
    return true;
}

auto Card::GetSuit() const noexcept -> Card::Suit {
    return suit_;
}

auto Card::GetValue() const noexcept -> int {
    return value_;
}

auto operator<<(std::ostream& os, Card c) -> std::ostream& {
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

const Card Card::empty(0, Card::Suit::Numeral, Card::Pip::Clubs);
