#include "card.hxx"

Card::Card(int value, Suit suit, Pip pip) noexcept
    : value_(value), suit_(suit), pip_(pip) {}

bool Card::SetValue(int new_value) noexcept {
    if (suit_ != Suit::Ace || (new_value != 1 && new_value != 11)) {
        return false;
    }
    value_ = new_value;
    return true;
}

Card::Suit Card::GetSuit() const noexcept {
    return suit_;
}

int Card::GetValue() const noexcept {
    return value_;
}

std::ostream& operator<<(std::ostream& os, Card c) {
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
