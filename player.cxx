#include <utility>

#include "player.hxx"

Player::Player(std::string name)
    : cards_(), name_(std::move(name)), win_count_(), loss_count_(), blackjack_count_(), bust_count_(), surrender_count_() {}

void Player::Init() {
    cards_.clear();
    ace_index_.clear();
}

void Player::AddCard(Card c) {
    cards_.push_back(c);
    if (c.GetSuit() == Card::Suit::Ace) {
        ace_index_.push_back(cards_.size() - 1);
    }
}

bool Player::SetAce(std::size_t i, int new_value) noexcept {
    return cards_[ace_index_[i]].SetValue(new_value);
}

void Player::Win() {
    win_count_++;
}

void Player::Lose() {
    loss_count_++;
}

void Player::Blackjack() {
    blackjack_count_++;
}

void Player::Bust() {
    bust_count_++;
}

void Player::Surrender() {
    surrender_count_++;
}

const std::string& Player::GetName() const noexcept {
    return name_;
}

int Player::GetCurrentScore() const noexcept {
    int result{};
    for (const auto& c : cards_) {
        result += c.GetValue();
    }
    return result;
}

std::size_t Player::GetAceCount() const noexcept {
    return ace_index_.size();
}

int Player::GetWinCount() const noexcept {
    return win_count_;
}

int Player::GetLossCount() const noexcept {
    return loss_count_;
}

int Player::GetBlackjackCount() const noexcept {
    return blackjack_count_;
}

int Player::GetBustCount() const noexcept {
    return bust_count_;
}

int Player::GetSurrenderCount() const noexcept {
    return surrender_count_;
}

void Player::WriteCurrentCards(std::ostream& os) const {
    for (auto c : cards_) {
        os << c << '\n';
    }
}
